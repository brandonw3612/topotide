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
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1955.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1972.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1976.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1980.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1982.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1986.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1988.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1989.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1990.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1992.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1994.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1996.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1997.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1998.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1999.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2000.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2001.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2002.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2003.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2004.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2005.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2006.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2007.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2008.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2009.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2010.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2011.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2012.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2013.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_2014.txt.ascii");
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

    auto prefix = "/Users/brandon/Desktop/1000_10/";

    context.mapAllFrames(prefix, 1000.0, 10.0);
    return 0;

    auto viewer = context.createMappingViewer(prefix, 1000.0, 10.0);
    viewer->show();
    return app.exec();
}
