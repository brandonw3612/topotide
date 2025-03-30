#include "Context.h"
#include "DualFrameViewer.h"
#include <QApplication>
#include <QGraphicsView>

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
    DualFrameViewer dfv;
    dfv.setFrames(fs);
    dfv.show();
    return app.exec();
}
