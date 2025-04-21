#include "Context.h"
#include "DualFrameViewer.h"
#include "Frame.h"
#include "PathFrame.h"
#include "SubdividedAbstractGraph.h"
#include <QApplication>
#include <QGraphicsView>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Context context;
    
    QString dataFolderPath = "/home/karel/Downloads/western-scheldt-data/";

    QStringList frames;
    frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1964.txt.ascii");
    // frames.append(dataFolderPath + "Westerschelde_ResampledGrid_1968.txt.ascii");
    context.openFrames(frames);
    context.openBoundary(dataFolderPath + "boundary4-corrected.txt");
    context.computeNetworkGraph();
    context.buildAbstractionForAllFrames();
    auto fs = context.getFrames();
    
    auto frameContainingPath = fs[0];
    auto parentChannel = frameContainingPath->getGraph()->getParentlessChannel(100.0);
    auto path = parentChannel->getPoints();
    auto pathFrame = std::make_shared<PathFrame>("Main path 1964", path, frameContainingPath->getGraph()->getBounds());

    auto subdividedGraph = SubdividedAbstractGraph(frameContainingPath->getGraph(), 10, 10000.0);
    std::cout << subdividedGraph.edgeDepth.size() << std::endl;

    std::vector<std::shared_ptr<Frame>> all_frames;
    for (auto& f : fs) {
        all_frames.push_back(std::static_pointer_cast<Frame>(f));
    }
    all_frames.push_back(std::static_pointer_cast<Frame>(pathFrame));
    
    DualFrameViewer dfv;
    dfv.setFrames(all_frames);
    dfv.show();
    return app.exec();
}
