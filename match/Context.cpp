#include "Context.h"

#include "boundaryreader.h"
#include "GraphComputer.h"

#include <iostream>
#include <set>
#include <QFileInfo>

#include "MappingViewer.h"
#include "io/esrigridreader.h"
#include "io/gdalreader.h"
#include "io/textfilereader.h"

#include "NetworkConverter.h"
#include "NetworkGraphEdgeConnector.h"
#include "PathMatcher.h"
#include "PrecomputedDisplayFrame.h"
#include "ReachMapResult.h"
#include "utils/ParallelComputer.hpp"
#include "utils/VectorUtils.hpp"
#include "SVG.h"

void Context::openFrames(QStringList &fileNames) {
    // Not reachable in CLI
    // if (m_computationRunning) {
    //     QMessageBox msgBox;
    //     msgBox.setIcon(QMessageBox::Critical);
    //     msgBox.setWindowTitle("Cannot open river");
    //     msgBox.setText("<qt>The river cannot be opened.");
    //     msgBox.setInformativeText("<qt>There is still a running computation. "
    //                               "Wait until the computation is finished, and "
    //                               "try again.");
    //     msgBox.exec();
    //     return;
    // }

    fileNames.sort();

    std::shared_ptr<RiverData> riverData;
    Units units;
    for (const QString &fileName: fileNames) {
        std::shared_ptr<RiverFrame> frame = loadFrame(fileName, units);
        if (!frame) {
            break;
        }
        if (riverData) {
            if (frame->m_heightMap.width() != riverData->width() ||
                frame->m_heightMap.height() != riverData->height()) {
                return;
            }
        } else {
            riverData =
                    std::make_shared<RiverData>(frame->m_heightMap.width(), frame->m_heightMap.height(), units);
        }
        riverData->addFrame(frame);
    }

    if (!riverData || riverData->frameCount() == 0) {
        m_riverData = nullptr;
        // map->setRiverData(nullptr);
        // map->setRiverFrame(nullptr);
        // updateActions();
        return;
    }
    m_riverData = riverData;
    // m_frame = 0;
    // map->setRiverData(m_riverData);
    // map->setRiverFrame(activeFrame());
    // map->resetTransform();
    // backgroundDock->setElevationRange(m_riverData->minimumElevation(), m_riverData->maximumElevation());
    // unitsDock->setUnits(m_riverData->units());
    // progressDock->reset();
    // timeDock->setFrame(0);
    // timeDock->setFrameCount(m_riverData->frameCount());
    // updateActions();
    // markComputationNeeded(false);

    if (fileNames.length() == 1) {
        std::cout << "Opened river \"" << fileNames[0].toStdString() << "\"" << std::endl << std::endl;
    } else {
        std::cout << QString("Opened river containing %1 frames").arg(fileNames.length()).toStdString()
                << std::endl;
    }
}

std::shared_ptr<RiverFrame> Context::loadFrame(const QString &fileName, Units &units) {
    HeightMap heightMap;
    QString error = "[no error given]";
    if (fileName.endsWith(".txt")) {
        heightMap = TextFileReader::readTextFile(fileName, error, units);
    } else if (fileName.endsWith(".ascii") || fileName.endsWith(".asc")) {
        heightMap = EsriGridReader::readGridFile(fileName, error, units);
    } else {
        heightMap = GdalReader::readGdalFile(fileName, error, units);
    }

    if (heightMap.isEmpty()) {
        // something went wrong
        // QMessageBox msgBox;
        // msgBox.setIcon(QMessageBox::Critical);
        // msgBox.setWindowTitle("Cannot open river");
        // msgBox.setText(QString("<qt>The river <code>%1</code> cannot be opened.").arg(fileName));
        // msgBox.setInformativeText("<qt><p>This file does not seem to be an image file "
        //                         "or a valid text file containing elevation "
        //                         "data.</p>");
        // msgBox.setDetailedText("Reading the file failed due to the "
        //                     "following error:\n    " + error);
        // msgBox.exec();
        std::cerr << "Cannot open river: "
                << QString("The river %1 cannot be opened.").arg(fileName).toStdString() << std::endl
                << "This file does not seem to be an image file or a valid text file containing elevation data."
                << std::endl << "Reading the text file failed due to the following error:" << std::endl
                << "\t" << error.toStdString() << std::endl << std::endl;
        return nullptr;
    }

    return std::make_shared<RiverFrame>(fileName, heightMap);
}

void Context::openBoundary(const QString &fileName) {
    // QString fileName = QFileDialog::getOpenFileName(
    //             this,
    //             "Open river boundary",
    //             ".",
    //             "Boundary text files (*.txt)");

    if (fileName == nullptr) {
        return;
    }

    // if (m_computationRunning) {
    //     QMessageBox msgBox;
    //     msgBox.setIcon(QMessageBox::Critical);
    //     msgBox.setWindowTitle("Cannot open boundary");
    //     msgBox.setText("<qt>The river boundary cannot be opened.");
    //     msgBox.setInformativeText("<qt>There is still a running computation. "
    //                               "Wait until the computation is finished, and "
    //                               "try again.");
    //     msgBox.exec();
    //     return;
    // }

    QString error = "";
    Boundary boundary =
            BoundaryReader::readBoundary(fileName, m_riverData->width(), m_riverData->height(), error);

    if (error != "") {
        // something went wrong
        // QMessageBox msgBox;
        // msgBox.setIcon(QMessageBox::Critical);
        // msgBox.setWindowTitle("Cannot open boundary");
        // msgBox.setText(QString("<qt>The river boundary <code>%1</code> cannot be opened.").arg(fileName));
        // msgBox.setInformativeText("<qt><p>This file does not seem to be "
        //                           "a valid text file containing a river "
        //                           "boundary.</p>");
        // msgBox.setDetailedText("Reading the text file failed due to the "
        //                        "following error:\n    " + error);
        // msgBox.exec();
        std::cerr << "Cannot open boundary: "
                << QString("The river boundary %1 cannot be opened.").arg(fileName).toStdString() << std::endl
                << "This file does not seem to be a valid text file containing a river boundary." << std::endl
                << "Reading the text file failed due to the following error:" << std::endl
                << "\t" << error.toStdString() << std::endl << std::endl;
        return;
    }

    m_riverData->setBoundary(boundary);

    std::cout << "Opened boundary \"" << fileName.toStdString() << "\"" << std::endl;
    // map->update();
    // markComputationNeeded(false);
}

void Context::computeNetworkGraph() {
    if (!m_riverData->boundaryRasterized().isValid()) {
        // QMessageBox msgBox;
        // msgBox.setIcon(QMessageBox::Critical);
        // msgBox.setWindowTitle("Boundary invalid");
        // msgBox.setText("<qt>The computation cannot run as the boundary is invalid.");
        // msgBox.setInformativeText("<qt>A valid boundary does not self-intersect and does not visit "
        //                           "any points more than once. "
        //                           "Edit the boundary and try again.");
        // msgBox.exec();
        std::cerr << "Boundary invalid: The computation cannot run as the boundary is invalid." << std::endl
                << "A valid boundary does not self-intersect and does not visit any points more than once. "
                << "Edit the boundary and try again." << std::endl << std::endl;
        return;
    }
    // progressDock->reset();
    // m_computationRunning = true;
    // m_computationNeeded = false;

    // activeFrame()->m_inputGraph = nullptr;
    // activeFrame()->m_inputDcel = nullptr;
    // activeFrame()->m_msComplex = nullptr;
    // activeFrame()->m_networkGraph = nullptr;
    // map->update();
    // updateActions();

    for (int i = 0; i < m_riverData->frameCount(); i++) {
        auto frame = m_riverData->getFrame(i);
        GraphComputer gc(frame->m_name.toStdString(), m_riverData, frame);
        gc.start();
    }

    std::cout << std::endl;
}

void Context::buildAbstractionForAllFrames() {
    for (auto i = 0; i < m_riverData->frameCount(); i++) {
        auto frame = m_riverData->getFrame(i);
        QFileInfo fileInfo(frame->m_name);
        std::cout << "Converting " << fileInfo.baseName().toStdString() << std::endl;
        auto rn = NetworkConverter::ng2rn(frame->m_networkGraph);
        m_frames.push_back(std::make_shared<NetworkDisplayFrame>(fileInfo.baseName(), rn));
    }
}

std::map<int, ReachMapResult> Context::mapNetworks(const std::shared_ptr<ReachNetwork> &n1,
    const std::shared_ptr<ReachNetwork> &n2, double adt) {
    auto ng2 = NetworkConverter::rn2ng(n2);
    int offset = n1->getNodes().begin()->first;
    std::set validNodes = { n1->getNodes().begin()->first };
    for (const auto &[id, node]: n1->getNodes()) {
        auto up = node->getUpstreamParent(), dp = node->getDownstreamParent();
        if (up == nullptr && dp == nullptr) continue;
        if (up != nullptr && !validNodes.contains(up->getNode()->getReach()->getIndex())) continue;
        if (dp != nullptr && !validNodes.contains(dp->getNode()->getReach()->getIndex())) continue;
        validNodes.insert(id);
    }

    std::map<int, std::future<ReachMapResult>> mappedPoints;
    ParallelComputer pc(8);
    for (int nodeIndex: validNodes) {
        auto matched = pc.run([=] {
            auto start = std::chrono::system_clock::now();
            std::time_t start_time = std::chrono::system_clock::to_time_t(start);
            auto reachPath = n1->getReachPath(nodeIndex);
            auto reachSegment = n1->getNodes()[nodeIndex]->getReach()->getPoints();
            auto match = PathMatcher::match(reachPath, ng2, adt);
            auto matchedSegmentIndex = PathMatcher::matchSegment(reachPath, reachSegment, match.path);
            auto end = std::chrono::system_clock::now();
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << "[" << nodeIndex - offset << "/" << validNodes.size() << "] Matched reach " << nodeIndex <<
                    " and the subsegment path with len " << matchedSegmentIndex.second - matchedSegmentIndex.first + 1 << " in " << duration << "ms." << std::endl;
            ReachMapResult result;
            result.reach = reachSegment;
            result.reachPath = reachPath;
            result.matchedSegmentRange = matchedSegmentIndex;
            result.matchedPath = match.path;
            result.matchedPathVertexIndices = match.vertexIndices;
            return result;
        });
        mappedPoints[nodeIndex] = std::move(matched);
    }
    pc.waitAll();
    std::map<int, ReachMapResult> results;
    for (auto &[nodeIndex, future]: mappedPoints) {
        auto result = future.get();
        auto node = n1->getNodes()[nodeIndex];
        auto up = node->getUpstreamParent(), dp = node->getDownstreamParent();
        auto [start, end] = result.matchedSegmentRange;
        std::vector resultMatchedSegment(result.matchedPath.begin() + start, result.matchedPath.begin() + end + 1);
        if (up != nullptr && results.contains(up->getNode()->getReach()->getIndex())) {
            auto upResult = results[up->getNode()->getReach()->getIndex()];
            int firstNotOnParent = VectorUtils::firstIndexWhere(result.matchedPathVertexIndices, [=](const auto& i) {
                return VectorUtils::firstIndexOf(upResult.matchedPath, result.matchedPath[i]) == -1;
            });
            if (firstNotOnParent <= 0 && VectorUtils::all(resultMatchedSegment, [=](const auto& p) {
                return VectorUtils::contains(upResult.matchedPath, p);
            })) {
                start = result.matchedPath.size();
            } else if (firstNotOnParent > 0 && start >= result.matchedPathVertexIndices[firstNotOnParent - 1]) {
                start = result.matchedPathVertexIndices[firstNotOnParent - 1];
            } else {
                int left = VectorUtils::lastIndexWhere(result.matchedPathVertexIndices, [=](const auto& i) {
                    return i < start;
                });
                auto leftI = result.matchedPathVertexIndices[left];
                auto leftP1I = result.matchedPathVertexIndices[left + 1];
                auto leftMid = leftI / 2 + leftP1I / 2;
                if (VectorUtils::contains(upResult.matchedPath, result.matchedPath[leftMid]) &&
                    VectorUtils::contains(upResult.matchedPath, result.matchedPath[leftP1I])) {
                    start = result.matchedPathVertexIndices[left + 1];
                } else {
                    start = result.matchedPathVertexIndices[left];
                }
            }
        }
        if (dp != nullptr && results.contains(dp->getNode()->getReach()->getIndex())) {
            auto dpResult = results[dp->getNode()->getReach()->getIndex()];
            int lastNotOnParent = VectorUtils::lastIndexWhere(result.matchedPathVertexIndices, [=](const auto& i) {
                return VectorUtils::lastIndexOf(dpResult.matchedPath, result.matchedPath[i]) == -1;
            });
            if ((lastNotOnParent < 0 || lastNotOnParent == result.matchedPathVertexIndices.size() - 1) && VectorUtils::all(resultMatchedSegment, [=](const auto& p) {
                return VectorUtils::contains(dpResult.matchedPath, p);
            })) {
                end = -1;
            } else if (lastNotOnParent >= 0 && lastNotOnParent < result.matchedPathVertexIndices.size() - 1 && end <= result.matchedPathVertexIndices[lastNotOnParent + 1]) {
                end = result.matchedPathVertexIndices[lastNotOnParent + 1];
            } else {
                int right = VectorUtils::firstIndexWhere(result.matchedPathVertexIndices, [=](const auto& i) {
                    return i > end;
                });
                auto rightI = result.matchedPathVertexIndices[right];
                auto rightM1I = result.matchedPathVertexIndices[right - 1];
                auto rightMid = rightI / 2 + rightM1I / 2;
                if (VectorUtils::contains(dpResult.matchedPath, result.matchedPath[rightMid]) &&
                    VectorUtils::contains(dpResult.matchedPath, result.matchedPath[rightM1I])) {
                    end = result.matchedPathVertexIndices[right - 1];
                } else {
                    end = result.matchedPathVertexIndices[right];
                }
            }
        }
        result.fixedSegmentRange = std::make_pair(start, end);
        results[nodeIndex] = result;
    }
    return results;
}

void Context::mapAllFrames(std::string outputFilePrefix, double sourceDeltaThreshold, double targetDeltaThreshold, double adt) {
    for (int i = 0; i < m_frames.size() - 1; i++) {
        std::cout << "Mapping frame " << i << " to frame " << i + 1 << std::endl;
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::endl << std::ctime(&now_time) << std::endl;
        auto n1 = m_frames[i]->getNetwork()->filter([sourceDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= sourceDeltaThreshold;
        });
        auto n2 = m_frames[i + 1]->getNetwork()->filter([targetDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= targetDeltaThreshold;
        });
        auto results = mapNetworks(n1, n2, adt);
        std::ofstream outFile(outputFilePrefix + std::to_string(i) + "_mapped.txt", std::ios::out);
        outFile << results.size() << std::endl;
        for (const auto& [nodeIndex, _] : results) {
            auto r = results[nodeIndex];
            outFile << nodeIndex << " " << r.reach.size() << " " << r.reachPath.size() << " " << r.matchedPath.size() << std::endl;
            // outFile << nodeIndex << " " << r.reach.size() << " " << r.reachPath.size() << " " << r.matchedPath.size() << " " << r.matchedPathVertexIndices.size() << std::endl;
            for (const auto &p: r.reach) {
                outFile << p.x << " " << p.y << " ";
            }
            outFile << std::endl;
            for (const auto &p: r.reachPath) {
                outFile << p.x << " " << p.y << " ";
            }
            outFile << std::endl;
            for (const auto &p: r.matchedPath) {
                outFile << p.x << " " << p.y << " ";
            }
            outFile << std::endl;
            // for (const auto i1: r.matchedPathVertexIndices) {
            //     outFile << i1 << " ";
            // }
            // outFile << std::endl;
            outFile << r.matchedSegmentRange.first << " " << r.matchedSegmentRange.second << " ";
            outFile << r.fixedSegmentRange.first << " " << r.fixedSegmentRange.second << std::endl;
        }
        outFile.close();
        now = std::chrono::system_clock::now();
        now_time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::endl << std::ctime(&now_time) << std::endl;
    }
}

MappingViewer* Context::createMappingViewer(const std::string &resultPrefix, double sourceDeltaThreshold, double targetDeltaThreshold) {
    std::vector<std::shared_ptr<PrecomputedDisplayFrame>> frames;
    for (int i = 0; i < m_frames.size() - 1; i++) {
        auto n1 = m_frames[i]->getNetwork()->filter([sourceDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= sourceDeltaThreshold;
        });
        auto n2 = m_frames[i + 1]->getNetwork()->filter([targetDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= targetDeltaThreshold;
        });
        std::cout << "Loading precomputed results for " << m_frames[i]->getName().toStdString() << std::endl;
        auto precomputed = PreComputedReachNetwork::createFrom(n1, resultPrefix + std::to_string(i) + "_mapped.txt");
        auto frame = std::make_shared<PrecomputedDisplayFrame>(
            m_frames[i]->getName() + "/" + m_frames[i + 1]->getName(),
            precomputed,
            n2
        );
        frames.push_back(frame);
    }
    return new MappingViewer(frames);
}

void Context::createSVGOutput(const std::string &resultPrefix, const std::string &outputPrefix,
                    double sourceDeltaThreshold, double targetDeltaThreshold) {
    for (int i = 0; i < m_frames.size() - 1; i++) {
        auto n1 = m_frames[i]->getNetwork()->filter([sourceDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= sourceDeltaThreshold;
        });
        auto n2 = m_frames[i + 1]->getNetwork()->filter([targetDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= targetDeltaThreshold;
        });
        auto precomputed = PreComputedReachNetwork::createFrom(n1, resultPrefix + std::to_string(i) + "_mapped.txt");

        SVG svg(outputPrefix, m_frames[i]->getName().toStdString(), m_frames[i + 1]->getName().toStdString(),
                                 sourceDeltaThreshold, targetDeltaThreshold, precomputed);
        svg.createMatchingOutputs(false);
        svg.createMatchingOutputs(true);
        // svg.createMatchingOutputsSeparated();
    }
}
