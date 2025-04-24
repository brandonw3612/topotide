#include <ranges>

#include "ReachNetwork.h"

ReachNetwork::Node::Node(
    const std::shared_ptr<ReachNetwork>& network,
    const std::shared_ptr<Reach>& reach
) {
    m_network = network;
    m_depth = 0;
    m_reach = reach;
}

std::shared_ptr<ReachNetwork> ReachNetwork::create() {
    auto network = std::shared_ptr<ReachNetwork>(new ReachNetwork());
    network->m_self = network;
    return network;
}

std::shared_ptr<ReachNetwork::Node> ReachNetwork::createNode(const std::shared_ptr<Reach> &reach) {
    auto node = std::shared_ptr<Node>(new Node(m_self, reach));
    m_nodes[reach->getIndex()] = node;
    return node;
}

void ReachNetwork::addEdge(int parentIndex, int childIndex, const Reach::IndexedPoint &intersection) {
    m_edges.emplace_back(std::make_shared<Edge>(parentIndex, childIndex, intersection));
    auto childNode = m_nodes[childIndex];
    auto parentNode = m_nodes[parentIndex];
    if (childNode->m_depth < parentNode->m_depth + 1) {
        childNode->m_depth = parentNode->m_depth + 1;
    }
}

std::pair<Point, Point> ReachNetwork::getBounds() const {
    auto bounds = m_nodes.begin()->second->getReach()->getBounds();
    for (const auto &n : std::views::values(m_nodes)) {
        auto b = n->getReach()->getBounds();
        if (b.first.x < bounds.first.x) bounds.first.x = b.first.x;
        if (b.first.y < bounds.first.y) bounds.first.y = b.first.y;
        if (b.second.x > bounds.second.x) bounds.second.x = b.second.x;
        if (b.second.y > bounds.second.y) bounds.second.y = b.second.y;
    }
    return bounds;
}

ReachNetwork::_::_(const Reach::IndexedPoint &intersection, const std::shared_ptr<Node> &node)
: m_intersection(intersection) {
    m_node = node;
}

std::vector<std::shared_ptr<ReachNetwork::Parent>> ReachNetwork::getParents(int index) {
    std::vector<std::shared_ptr<ReachNetwork::Parent>> result;
    for (const auto& edge : m_edges) {
        if (edge->getChildIndex() == index) {
            auto node = m_nodes[edge->getParentIndex()];
            result.emplace_back(std::make_shared<Parent>(edge->getIntersection(), node));
        }
    }
    return result;
}

std::vector<std::shared_ptr<ReachNetwork::Parent>> ReachNetwork::Node::getParents() const {
    return m_network->getParents(m_reach->getIndex());
}

std::vector<std::shared_ptr<ReachNetwork::Child> > ReachNetwork::getChildren(int index) {
    std::vector<std::shared_ptr<ReachNetwork::Child>> result;
    for (const auto& edge : m_edges) {
        if (edge->getParentIndex() == index) {
            auto node = m_nodes[edge->getChildIndex()];
            result.emplace_back(std::make_shared<Child>(edge->getIntersection(), node));
        }
    }
    return result;
}

std::vector<std::shared_ptr<ReachNetwork::Child>> ReachNetwork::Node::getChildren() const {
    return m_network->getChildren(m_reach->getIndex());
}