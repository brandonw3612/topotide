#ifndef TOPOTIDE_PATHSIMILARITYCOMPUTER_H
#define TOPOTIDE_PATHSIMILARITYCOMPUTER_H

#include <vector>
#include <unordered_set>

#include "networkgraph.h"
#include "point.h"

class PathSimilarityComputer {
private:
    class StatusStructure {
        public:
        double lowestDTW = __DBL_MAX__;
        std::vector<int> verticesPath;
    };

private:
    std::shared_ptr<NetworkGraph> m_graph;

public:
    explicit PathSimilarityComputer(const std::shared_ptr<NetworkGraph>& graph) { m_graph = graph; }
    std::vector<Point> computeMostSimilarPath(std::vector<Point> inputPath);
private:
    void recursiveComputeMostSimilarPath(NetworkGraph::Edge& edge, std::unordered_set<int> verticesInsidePath,
                                     std::vector<int> verticesPath, std::vector<double> DTW_init,
                                    StatusStructure& statusStructure, std::vector<Point>& inputPath);
};

#endif // PATHSIMILARITYCOMPUTER_H