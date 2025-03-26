#include "Context.h"
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    Context context;
    QStringList frames;
    QString dataFolderPath = "/Users/brandon/Downloads/western-scheldt-data/";
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();
    return 0;
}
