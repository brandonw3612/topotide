#ifndef EDGECONNECTOR_H
#define EDGECONNECTOR_H

#include <vector>

#include "networkgraph.h"
#include "Reach.h"

#define ConstReachBuilderIterator const std::vector<std::shared_ptr<Reach::Builder>>::iterator&

class NetworkGraphEdgeConnector {
private:
    std::vector<std::shared_ptr<Reach::Builder>> m_reachBuilders;
    std::vector<NetworkGraph::Edge> m_edges;

private:
    explicit NetworkGraphEdgeConnector(const std::vector<NetworkGraph::Edge>& edges);
    void connectAllEdges();
    void tryMergeReach(ConstReachBuilderIterator reachIterator);

public:
    static std::vector<std::shared_ptr<Reach>> connect(const std::vector<NetworkGraph::Edge>& edges);
};



#endif //EDGECONNECTOR_H
