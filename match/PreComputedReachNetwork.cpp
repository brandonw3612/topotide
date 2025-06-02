#include "PreComputedReachNetwork.h"

#include<ranges>
#include<fstream>

std::shared_ptr<ReachNetwork> PreComputedReachNetwork::asReachNetwork() {
    return m_self;
}

std::ranges::elements_view<std::ranges::ref_view<std::map<int, PreComputedReachNetwork::Precomputed>>, 0> PreComputedReachNetwork::getMappedReachIndices() {
    return m_precomputed | std::views::keys;
}

std::vector<Point> PreComputedReachNetwork::getReachPath(int reachIndex) {
    return m_precomputed[reachIndex].reachPath;
}

std::vector<Point> PreComputedReachNetwork::getMatchedSegment(int reachIndex) {
    // return m_precomputed[reachIndex].matchedSegment;
    return m_precomputed[reachIndex].fixedSegment;
}

std::vector<Point> PreComputedReachNetwork::getMatchedPath(int reachIndex) {
    return m_precomputed[reachIndex].matchedPath;
}

std::shared_ptr<PreComputedReachNetwork> PreComputedReachNetwork::createFrom(
    const std::shared_ptr<ReachNetwork> &reachNetwork, const std::string &filePath) {
    auto network = std::shared_ptr<PreComputedReachNetwork>(new PreComputedReachNetwork());
    network->m_self = std::static_pointer_cast<ReachNetwork>(network);
    for (auto node: reachNetwork->m_nodes | std::views::values) {
        network->createNode(node->getReach());
    }
    for (const auto& edge : reachNetwork->m_upstreamEdges) {
        network->addEdge(Upstream, edge->getParentIndex(), edge->getChildIndex(), edge->getIntersection());
    }
    for (const auto& edge : reachNetwork->m_downstreamEdges) {
        network->addEdge(Downstream, edge->getParentIndex(), edge->getChildIndex(), edge->getIntersection());
    }
    for (const auto &[id, node] : reachNetwork->m_nodes) {
        network->m_nodes[id]->m_flowDirection = node->m_flowDirection;
    }
    std::ifstream in(filePath);
    int numberOfNodes;
    in >> numberOfNodes;
    while (numberOfNodes--) {
        int nodeIndex, reachSize, reachPathSize, matchedPathSize;
        in >> nodeIndex >> reachSize >> reachPathSize >> matchedPathSize;
        Precomputed p;
        p.nodeIndex = nodeIndex;
        readPoints(in, reachSize);
        p.reachPath = readPoints(in, reachPathSize);
        p.matchedPath = readPoints(in, matchedPathSize);
        int matchStart, matchEnd, fixedStart, fixedEnd;
        in >> matchStart >> matchEnd >> fixedStart >> fixedEnd;
        p.matchedSegment = std::vector<Point>(p.matchedPath.begin() + matchStart, p.matchedPath.begin() + matchEnd + 1);
        p.fixedSegment = std::vector<Point>(p.matchedPath.begin() + fixedStart, p.matchedPath.begin() + fixedEnd + 1);
        network->m_precomputed[nodeIndex] = p;
    }
    return network;

}

std::shared_ptr<PreComputedReachNetwork> PreComputedReachNetwork::create() {
    auto network = std::shared_ptr<PreComputedReachNetwork>(new PreComputedReachNetwork());
    network->m_self = std::static_pointer_cast<ReachNetwork>(network);
    return network;
}

Point PreComputedReachNetwork::readPoint(std::ifstream &in) {
    double x, y;
    in >> x >> y;
    return Point(x, y, 0);
}

std::vector<Point> PreComputedReachNetwork::readPoints(std::ifstream &in, int size) {
    std::vector<Point> points(size);
    for (int i = 0; i < size; ++i) {
        points[i] = readPoint(in);
    }
    return points;
}