#include "NetworkGraphEdgeConnector.h"

NetworkGraphEdgeConnector::NetworkGraphEdgeConnector(const std::vector<NetworkGraph::Edge> &edges) {
    m_edges = edges;
}

void NetworkGraphEdgeConnector::connectAllEdges() {
    m_reachBuilders.clear();
    for (auto const &edge : m_edges) {
        if (m_reachBuilders.empty()) {
            m_reachBuilders.push_back(std::make_shared<Reach::Builder>(edge));
        } else {
            bool appended = false;
            for (auto c = m_reachBuilders.begin(); c != m_reachBuilders.end(); ++c) {
                if (c->get()->appendEdge(edge)) {
                    appended = true;
                    tryMergeReach(c);
                    break;
                }
            }
            if (!appended) {
                m_reachBuilders.push_back(std::make_shared<Reach::Builder>(edge));
            }
        }
    }
}

void NetworkGraphEdgeConnector::tryMergeReach(ConstReachBuilderIterator reachIterator) {
    const auto rb = std::move(*reachIterator);
    m_reachBuilders.erase(reachIterator);
    for (auto c = m_reachBuilders.begin(); c != m_reachBuilders.end(); c++) {
        if (c->get()->pendReach(rb)) {
            tryMergeReach(c);
            return;
        }
    }
    m_reachBuilders.push_back(rb);
}


std::vector<std::shared_ptr<Reach>> NetworkGraphEdgeConnector::connect(const std::vector<NetworkGraph::Edge> &edges) {
    auto connector = std::shared_ptr<NetworkGraphEdgeConnector>(new NetworkGraphEdgeConnector(edges));
    connector->connectAllEdges();
    std::vector<std::shared_ptr<Reach>> result;
    for (auto& rb : connector->m_reachBuilders) {
        result.push_back(rb->build());
    }
    return result;
}
