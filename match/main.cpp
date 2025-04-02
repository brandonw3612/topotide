#include "Context.h"
#include "DualFrameViewer.h"
#include <QApplication>
#include <QGraphicsView>
#include "ChannelMatching.h"
#include <iostream>
#include <fstream>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Context context;
    QStringList frames;
    QString dataFolderPath = "/Users/brandon/Downloads/western-scheldt-data/";
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1955.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1972.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1976.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1980.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1982.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1986.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1989.txt.ascii");
//    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1990.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();
    auto fs = context.getFrames();
    auto mc1 = fs[1]->getGraph()->getNodes()[0]->getChannel();
    auto mc2 = fs[2]->getGraph()->getNodes()[0]->getChannel();
    auto dtw_res = ChannelMatching::DTWMatchList(mc1, mc2);
    std::ofstream file;
    file.open ("./dtw_compare.txt");
    file <<"Listing the mapping from points of 1964 dataset to those of 1968 dataset\n";
    auto p1 = mc1->getPoints();
    double totalCost = 0;
    for (int i = 0; i < dtw_res.size(); i++) {
        auto p2 = dtw_res[i].first;
        auto cost = dtw_res[i].second;
        totalCost += cost;
        file << "Matched ("<< p1[i].x << ", " << p1[i].y << ") to point (" << p2.x << ", " << p2.y <<") with cost = "<< cost << "\n";
    }
    file << "Total cost = " << totalCost << "\n";
    file.close();

    DualFrameViewer dfv;
    dfv.setFrames(fs);
    dfv.show();
    return app.exec();
}
