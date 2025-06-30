#include <chrono>
#include <fstream>
#include <QApplication>

#include "Context.h"
#include "DualFrameViewer.h"
#include "PathFrame.h"

void printTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&now_time) << std::endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Context context;

    QString dataFolderPath = "/home/karel/Downloads/western-scheldt-data/";

    // auto begin = std::chrono::steady_clock::now();
    QStringList frames;
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();

    // auto fs = context.getFrames();
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
    // return app.exec();

    auto resultPrefix = "/home/karel/Downloads/western-scheldt-data/";
    auto SVGPrefix = "/home/karel/Downloads/western-scheldt-data/SVG/";

    context.mapAllFrames(resultPrefix, 50.0, 5.0, 20.0);
    context.createSVGOutput(resultPrefix, SVGPrefix, 50.0, 2.0);

    return 0;

    auto viewer = context.createMappingViewer(resultPrefix, 50.0);
    viewer->show();
    return app.exec();
}
