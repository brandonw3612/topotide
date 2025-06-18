#include "Context.h"

#include "boundaryreader.h"
#include "GraphComputer.h"

#include <iostream>
#include <set>
#include <QFileInfo>
#include <set>

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
        result.fixedSegmentRange = getConnectedStartAndEndIndices(nodeIndex, result, results, n1, n2);
        results[nodeIndex] = result;
    }
    return results;
}

std::pair<int, int> Context::getConnectedStartAndEndIndices(int nodeIndex, ReachMapResult& result, std::map<int, ReachMapResult>& results,
                    const std::shared_ptr<ReachNetwork>& n1, const std::shared_ptr<ReachNetwork> &n2) {
    auto node = n1->getNodes()[nodeIndex];
    auto up = node->getUpstreamParent(), dp = node->getDownstreamParent();
    auto [start, end] = result.matchedSegmentRange;

    std::set<Point> validStopPoints;
    if (up != nullptr) {
        auto upResult = results[up->getNode()->getReach()->getIndex()];
        for (auto vertexIndex : result.matchedPathVertexIndices) {
            auto vertexPoint = result.matchedPath[vertexIndex];
            if (VectorUtils::firstIndexOf(upResult.matchedPath, vertexPoint) != -1) {
                validStopPoints.insert(vertexPoint);
            }
        }
    }
    if (dp != nullptr) {
        auto downResult = results[dp->getNode()->getReach()->getIndex()];
        for (auto vertexIndex : result.matchedPathVertexIndices) {
            auto vertexPoint = result.matchedPath[vertexIndex];
            if (VectorUtils::firstIndexOf(downResult.matchedPath, vertexPoint) != -1) {
                validStopPoints.insert(vertexPoint);
            }
        }
    }
    if (dp == nullptr && up == nullptr) {
        return {0, result.matchedPath.size() - 1};
    }
    if (dp == nullptr || up == nullptr) {
        validStopPoints.insert(result.matchedPath[0]);
        validStopPoints.insert(result.matchedPath.back());
    }
    int start1 = start, start2 = start, end1 = end, end2 = end;
    while (start1 > 0) {
        if (validStopPoints.count(result.matchedPath[start1])) {
            break;
        }
        start1--;
    }
    while (start2 < result.matchedPath.size() - 1) {
        if (validStopPoints.count(result.matchedPath[start2])) {
            break;
        }
        start2++;
    }
    while (end1 > 0) {
        if (validStopPoints.count(result.matchedPath[end1])) {
            break;
        }
        end1--;
    }
    while (end2 < result.matchedPath.size() - 1) {
        if (validStopPoints.count(result.matchedPath[end2])) {
            break;
        }
        end2++;
    }

    std::vector<std::pair<int, int>> validPairs;
    if (up == nullptr) std::swap(dp, up);
    if (dp == nullptr) { // Now only dp is nullptr
        for (auto testStart : {start1, start2}) {
            for (auto testEnd : {end1, end2}) {
                if (testStart >= testEnd) continue;
                auto upResult = results[up->getNode()->getReach()->getIndex()];
                auto vertexPointStart = result.matchedPath[testStart];
                auto vertexPointEnd = result.matchedPath[testEnd];
                if ((VectorUtils::firstIndexOf(upResult.matchedPath, vertexPointStart) != -1) ^
                    (VectorUtils::firstIndexOf(upResult.matchedPath, vertexPointEnd) != -1)) { // Only one end should overlap
                    validPairs.push_back({testStart, testEnd});
                }
            }
        }
        int cost = INT_MAX;
        std::pair<int, int> bestPair;
        for (auto& validPair : validPairs) {
            int checkCost = std::abs(validPair.first - start) + std::abs(validPair.second - end);
            if (checkCost < cost) {
                cost = checkCost;
                bestPair = validPair;
            }
        }
        if (cost == INT_MAX) {
            return {start1, end2}; // Preferably, this does not happen, but it does happen that in some cases the reach parent becomes unreachable
        }
        return bestPair;
    } else { // Both are present
        for (auto testStart : {start1, start2}) {
            for (auto testEnd : {end1, end2}) {
                if (testStart >= testEnd) continue;
                auto downResult = results[dp->getNode()->getReach()->getIndex()];
                auto upResult = results[up->getNode()->getReach()->getIndex()];
                auto vertexPointStart = result.matchedPath[testStart];
                auto vertexPointEnd = result.matchedPath[testEnd];

                bool startMatchedWithUp = VectorUtils::firstIndexOf(upResult.matchedPath, vertexPointStart) != -1;
                bool startMatchedWithDown = VectorUtils::firstIndexOf(downResult.matchedPath, vertexPointStart) != -1;
                bool endMatchedWithUp = VectorUtils::firstIndexOf(upResult.matchedPath, vertexPointEnd) != -1;
                bool endMatchedWithDown = VectorUtils::firstIndexOf(downResult.matchedPath, vertexPointEnd) != -1;

                if ((startMatchedWithDown) &&
                    (endMatchedWithUp)) { // Only one end should overlap
                    validPairs.push_back({testStart, testEnd});
                } else if ((startMatchedWithUp) &&
                    (endMatchedWithDown)) {
                    validPairs.push_back({testStart, testEnd});
                }
            }
        }

        int cost = INT_MAX;
        std::pair<int, int> bestPair;
        for (auto& validPair : validPairs) {
            int checkCost = std::abs(validPair.first - start) + std::abs(validPair.second - end);
            if (checkCost < cost) {
                cost = checkCost;
                bestPair = validPair;
            }
        }
        if (cost == INT_MAX) {
            return {start1, end2}; // Preferably, this does not happen, but it does happen that in some cases the reach parent becomes unreachable
        }
        return bestPair;
    }
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
            outFile << r.matchedSegmentRange.first << " " << r.matchedSegmentRange.second << " ";
            outFile << r.fixedSegmentRange.first << " " << r.fixedSegmentRange.second << std::endl;
        }
        outFile.close();
        now = std::chrono::system_clock::now();
        now_time = std::chrono::system_clock::to_time_t(now);
        std::cout << std::endl << std::ctime(&now_time) << std::endl;
    }
}

MappingViewer* Context::createMappingViewer(const std::string &resultPrefix, double sourceDeltaThreshold) {
    std::vector<std::shared_ptr<PrecomputedDisplayFrame>> frames;
    for (int i = 0; i < m_frames.size() - 1; i++) {
        auto n1 = m_frames[i]->getNetwork()->filter([sourceDeltaThreshold](const auto &node) {
            return node->getReach()->getDelta() >= sourceDeltaThreshold;
        });
        std::cout << "Loading precomputed results for " << m_frames[i]->getName().toStdString() << std::endl;
        auto precomputed = PreComputedReachNetwork::createFrom(n1, resultPrefix + std::to_string(i) + "_mapped.txt");
        auto frame = std::make_shared<PrecomputedDisplayFrame>(
            m_frames[i]->getName() + "/" + m_frames[i + 1]->getName(),
            precomputed
        );
        frames.push_back(frame);
    }
    return new MappingViewer(frames);
}
