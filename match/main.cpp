#include <chrono>
#include <fstream>
#include <QApplication>

#include "Context.h"
#include "DualFrameViewer.h"
#include "Frame.h"
#include "MatchResultRenderer.h"
#include "NetworkConverter.h"
#include "utils/ParallelComputer.hpp"
#include "PathFrame.h"
#include "PathMatcher.h"
#include "PrecomputedDisplayFrame.h"
#include "PreComputedReachNetwork.h"

struct ReachMapResult {
    std::vector<Point> reach;
    std::vector<Point> reachPath;
    std::vector<Point> matchedSegment;
    std::vector<Point> matchedPath;
};

void printTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&now_time) << std::endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Context context;

    QString dataFolderPath = "/Users/brandon/Downloads/western-scheldt-data/";

    // auto begin = std::chrono::steady_clock::now();
    QStringList frames;
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();
    auto fs = context.getFrames();
    // auto end = std::chrono::steady_clock::now();
    // std::cout << "Time for abstraction graph: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;

    // std::vector<std::shared_ptr<Frame>> all_frames;
    // for (auto& f : fs) {
    //     all_frames.push_back(std::static_pointer_cast<Frame>(f));
    // }
    // all_frames.push_back(std::static_pointer_cast<Frame>(pathFrame));
    // all_frames.push_back(std::static_pointer_cast<Frame>(matchedPathFrame));

    // DualFrameViewer dfv;
    // dfv.setFrames(fs);
    // dfv.show();

    auto rn1964 = fs[0]->getNetwork(), rn1968 = fs[1]->getNetwork();
    auto rnFiltered1964 = rn1964->filter(
        [=](const std::shared_ptr<ReachNetwork::Node> &node) {
            return node->getReach()->getDelta() >= 2;
        });
    // auto rnFiltered1968 = rn1968->filter(
    //     [=](const std::shared_ptr<ReachNetwork::Node> &node) {
    //         return node->getReach()->getDelta() >= 2;
    //     });
    // auto ngFiltered1968 = NetworkConverter::rn2ng(rnFiltered1968);
    // std::set validNodes = {0};
    // for (const auto &[id, node]: rnFiltered1964->getNodes()) {
    //     auto up = node->getUpstreamParent(), dp = node->getDownstreamParent();
    //     if (up == nullptr && dp == nullptr) continue;
    //     if (up != nullptr && !validNodes.contains(up->getNode()->getReach()->getIndex())) continue;
    //     if (dp != nullptr && !validNodes.contains(dp->getNode()->getReach()->getIndex())) continue;
    //     validNodes.insert(id);
    // }
    // std::map<int, std::future<ReachMapResult>> mappedPoints;
    // printTime();
    // ParallelComputer pc(8);
    // for (int nodeIndex: validNodes) {
    //     auto matched = pc.run([=]() {
    //         auto reachPath = rnFiltered1964->getReachPath(nodeIndex);
    //         auto reachSegment = rnFiltered1964->getNodes()[nodeIndex]->getReach()->getPoints();
    //         auto matchedPath = PathMatcher::match(reachPath, ngFiltered1968, 10000.0);
    //         auto matchedSegment = PathMatcher::matchSegment(reachPath, reachSegment, matchedPath);
    //         std::cout << "Matched reach " << nodeIndex <<
    //                 " and the subsegment path with len " << matchedSegment.size() << std::endl;
    //         ReachMapResult result;
    //         result.reach = reachSegment;
    //         result.reachPath = reachPath;
    //         result.matchedSegment = matchedSegment;
    //         result.matchedPath = matchedPath;
    //         return result;
    //     });
    //     mappedPoints[nodeIndex] = std::move(matched);
    // }
    // pc.waitAll();
    // std::ofstream outFile("/Users/brandon/Desktop/matchedPaths.txt", std::ios::out);
    // outFile << validNodes.size() << std::endl;
    // for (int nodeIndex : validNodes) {
    //     auto r = mappedPoints[nodeIndex].get();
    //     outFile << nodeIndex << " " << r.reach.size() << " " << r.reachPath.size() << " " << r.matchedSegment.size() << " " << r.matchedPath.size() << std::endl;
    //     for (const auto &p: r.reach) {
    //         outFile << p.x << " " << p.y << " ";
    //     }
    //     outFile << std::endl;
    //     for (const auto &p: r.reachPath) {
    //         outFile << p.x << " " << p.y << " ";
    //     }
    //     outFile << std::endl;
    //     for (const auto &p: r.matchedSegment) {
    //         outFile << p.x << " " << p.y << " ";
    //     }
    //     outFile << std::endl;
    //     for (const auto &p: r.matchedPath) {
    //         outFile << p.x << " " << p.y << " ";
    //     }
    //     outFile << std::endl;
    // }
    // outFile.close();
    // printTime();
    // return 0;

    auto precomputed = PreComputedReachNetwork::createFrom(rnFiltered1964, "/Users/brandon/Desktop/matchedPaths.txt");
    auto frame = std::make_shared<PrecomputedDisplayFrame>("", precomputed);
    MatchResultRenderer renderer(frame);
    renderer.show();
    return app.exec();
}
