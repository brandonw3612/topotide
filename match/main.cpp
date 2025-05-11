#include <chrono>
#include <QApplication>

#include "Context.h"
#include "DualFrameViewer.h"
#include "Frame.h"
#include "NetworkConverter.h"
#include "PathFrame.h"
#include "PathMatcher.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Context context;
    
    QString dataFolderPath = "/home/karel/Downloads/western-scheldt-data/";

    auto begin = std::chrono::steady_clock::now();
    QStringList frames;
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();
    auto fs = context.getFrames();
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time for abstraction graph: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;

    begin = std::chrono::steady_clock::now();
    auto network1964 = fs[0]->getNetwork();
    const auto infiniteDeltaFilter = [](const auto& n) { return !std::isfinite(n->getReach()->getDelta()); };
    auto infReach = network1964->filter(infiniteDeltaFilter)->getNodes().front()->getReach();
    auto path = infReach->getPoints();
    auto pathFrame = std::make_shared<PathFrame>("Main path 1964", path, network1964->getBounds());
    end = std::chrono::steady_clock::now();
    std::cout << "Time for subdivided graph: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    
    begin = std::chrono::steady_clock::now();
    const auto network1968 = fs[1]->getNetwork();
    const auto deltaFilter = [](const auto& n) { return n->getReach()->getDelta() > 1; };
    const auto network1968Filtered = network1968->filter(deltaFilter);
    // auto matchedPath = PathMatcher::match(path, NetworkConverter::rn2ng(network1968Filtered), std::numeric_limits<double>::infinity());
    auto matchedPath = PathMatcher::match(path, NetworkConverter::rn2ng(network1968Filtered), 10.0);
    auto matchedPathFrame = std::make_shared<PathFrame>("Matched path 1968", matchedPath, network1968->getBounds());
    end = std::chrono::steady_clock::now();
    std::cout << "Time for path matching: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;

    std::vector<std::shared_ptr<Frame>> all_frames;
    for (auto& f : fs) {
        all_frames.push_back(std::static_pointer_cast<Frame>(f));
    }
    all_frames.push_back(std::static_pointer_cast<Frame>(pathFrame));
    all_frames.push_back(std::static_pointer_cast<Frame>(matchedPathFrame));
    
    DualFrameViewer dfv;
    dfv.setFrames(all_frames);
    dfv.show();
    return app.exec();
}
