#include "Context.h"
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    Context context;
    QStringList frames;
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1955.txt.ascii");
    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1968.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1972.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1976.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1980.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1982.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1986.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1988.txt.ascii");
//    frames.append("/Users/brandon/Downloads/western-scheldt-data/Westerschelde_ResampledGrid_1989.txt.ascii");
    context.openFrames(frames);
    context.openBoundary("/Users/brandon/Downloads/western-scheldt-data/boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.displayMainChannelScale();
    return 0;
}
