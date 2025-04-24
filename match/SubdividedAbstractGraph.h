#ifndef TOPOTIDE_SUBDIVIDEDABSTRACTGRAPH_H
#define TOPOTIDE_SUBDIVIDEDABSTRACTGRAPH_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "networkgraph.h"
#include "AbstractGraph.h"

class SubdividedAbstractGraph {
public:
    SubdividedAbstractGraph(std::shared_ptr<AbstractGraph> abstractGraph, int maxDepth, double minDelta);

public:
    NetworkGraph m_networkGraph;
    std::unordered_map<int, std::vector<int>> adjacencyList; // Vertex to adjacent edge IDs
    std::unordered_map<int, int> edgeDepth; // Edge ID to depth

private:
    void insertEdge(std::vector<Point>& points, std::map<Point, int> pointToID, int startIndex, int endIndex, int depth, double delta);
};

#endif // SUBDIVIDEDABSTRACTGRAPH_H