#ifndef TOPOTIDE_PATHSIMILARITYCOMPUTER_H
#define TOPOTIDE_PATHSIMILARITYCOMPUTER_H

#include <vector>
#include <unordered_set>
#include "point.h"
#include "SubdividedAbstractGraph.h"

class PathSimilarityComputer {
private:
    class StatusStructure {
        public:
        double lowestDTW = __DBL_MAX__;
        std::vector<int> verticesPath;
    };

private:
    SubdividedAbstractGraph m_graph;

public:
    PathSimilarityComputer(SubdividedAbstractGraph& graph) : m_graph(graph) {}
    std::vector<Point> computeMostSimilarPath(std::vector<Point> inputPath);
private:
    void recursiveComputeMostSimilarPath(NetworkGraph::Edge& edge, std::unordered_set<int> verticesInsidePath,
                                     std::vector<int> verticesPath, std::vector<double> DTW_init,
                                    StatusStructure& statusStructure, std::vector<Point>& inputPath);
};

#endif // PATHSIMILARITYCOMPUTER_H