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
    
    QString dataFolderPath = "/Users/brandon/Downloads/western-scheldt-data/";

    auto begin = std::chrono::steady_clock::now();
    QStringList frames;
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1972.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1976.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1980.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1982.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1986.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1988.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1989.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1990.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1992.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1994.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1996.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1997.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1998.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1999.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2000.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();
    auto fs = context.getFrames();
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time for abstraction graph: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;

    // begin = std::chrono::steady_clock::now();
    // auto network1964 = fs[0]->getNetwork();
    // const auto infiniteDeltaFilter = [](const auto& n) { return !std::isfinite(n->getReach()->getDelta()); };
    // auto infReach = network1964->filter(infiniteDeltaFilter)->getNodes().front()->getReach();
    // auto path = infReach->getPoints();
    // auto pathFrame = std::make_shared<PathFrame>("Main path 1964", path, network1964->getBounds());
    // end = std::chrono::steady_clock::now();
    // std::cout << "Time for subdivided graph: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    //
    // begin = std::chrono::steady_clock::now();
    // const auto network1968 = fs[1]->getNetwork();
    // const auto deltaFilter = [](const auto& n) { return n->getReach()->getDelta() > 5000; };
    // const auto network1968Filtered = network1968->filter(deltaFilter);
    // auto matchedPath = PathMatcher::match(path, NetworkConverter::rn2ng(network1968Filtered));
    // auto matchedPathFrame = std::make_shared<PathFrame>("Matched path 1968", matchedPath, network1968->getBounds());
    // end = std::chrono::steady_clock::now();
    // std::cout << "Time for path matching: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    //
    std::vector<std::shared_ptr<Frame>> all_frames;
    for (auto& f : fs) {
        all_frames.push_back(std::static_pointer_cast<Frame>(f));
    }
    // all_frames.push_back(std::static_pointer_cast<Frame>(pathFrame));
    // all_frames.push_back(std::static_pointer_cast<Frame>(matchedPathFrame));
    
    DualFrameViewer dfv;
    dfv.setFrames(fs);
    dfv.show();
    return app.exec();
}
