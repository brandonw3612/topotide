#include "Context.h"
#include "DualFrameViewer.h"
#include "Frame.h"
#include "PathFrame.h"
#include "SubdividedAbstractGraph.h"
#include "PathSimilarityComputer.h"
#include <QApplication>
#include <QGraphicsView>
#include <chrono>

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
    auto frameContainingPath = fs[0];
    auto parentChannel = frameContainingPath->getGraph()->getParentlessChannel(100.0);
    auto path = parentChannel->getPoints();
    auto pathFrame = std::make_shared<PathFrame>("Main path 1964", path, frameContainingPath->getGraph()->getBounds());
    end = std::chrono::steady_clock::now();
    std::cout << "Time for subdivided graph: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds" << std::endl;
    
    begin = std::chrono::steady_clock::now();
    auto frameContaingGraph = fs[1];
    auto subdividedGraph = SubdividedAbstractGraph(frameContaingGraph->getGraph(), 10, 10000.0);
    auto pathSimilarityComputer = PathSimilarityComputer(subdividedGraph);
    std::vector<Point> matchedPath = pathSimilarityComputer.computeMostSimilarPath(path);
    auto matchedPathFrame = std::make_shared<PathFrame>("Matched path 1968", matchedPath, frameContaingGraph->getGraph()->getBounds());
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
