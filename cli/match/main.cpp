#include <Context.h>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    Context context;
    QStringList frames;
    frames.append("/home/brandon/western-scheldt-data/Westerschelde_ResampledGrid_1955.txt.ascii");
    frames.append("/home/brandon/western-scheldt-data/Westerschelde_ResampledGrid_1964.txt.ascii");
    frames.append("/home/brandon/western-scheldt-data/Westerschelde_ResampledGrid_1968.txt.ascii");
    frames.append("/home/brandon/western-scheldt-data/Westerschelde_ResampledGrid_1972.txt.ascii");
    frames.append("/home/brandon/western-scheldt-data/Westerschelde_ResampledGrid_1976.txt.ascii");
    context.openFrames(frames);
    context.openBoundary("/home/brandon/western-scheldt-data/boundary4-corrected.txt");
    context.compute();
    return 0;
}
