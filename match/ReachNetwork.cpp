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

void ReachNetwork::addEdge(Connection connection, int parentIndex, int childIndex, const Reach::IndexedPoint &intersection) {
    if (connection == Upstream) {
        m_upstreamEdges.emplace_back(std::make_shared<Edge>(parentIndex, childIndex, intersection));
    } else {
        m_downstreamEdges.emplace_back(std::make_shared<Edge>(parentIndex, childIndex, intersection));
    }
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

ReachNetwork::_::_(Connection connection, const Reach::IndexedPoint &intersection, const std::shared_ptr<Node> &node)
: m_connection(connection), m_intersection(intersection) {
    m_node = node;
}

std::shared_ptr<ReachNetwork::Parent> ReachNetwork::getUpstreamParent(int index) {
    for (const auto& edge : m_upstreamEdges) {
        if (edge->getChildIndex() == index) {
            auto node = m_nodes[edge->getParentIndex()];
            return std::make_shared<Parent>(Upstream, edge->getIntersection(), node);
        }
    }
    return nullptr;
}

std::shared_ptr<ReachNetwork::Parent> ReachNetwork::getDownstreamParent(int index) {
    for (const auto& edge : m_downstreamEdges) {
        if (edge->getChildIndex() == index) {
            auto node = m_nodes[edge->getParentIndex()];
            return std::make_shared<Parent>(Downstream, edge->getIntersection(), node);
        }
    }
    return nullptr;
}

std::shared_ptr<ReachNetwork::Parent> ReachNetwork::Node::getUpstreamParent() const {
    return m_network->getUpstreamParent(m_reach->getIndex());
}

std::shared_ptr<ReachNetwork::Parent> ReachNetwork::Node::getDownstreamParent() const {
    return m_network->getDownstreamParent(m_reach->getIndex());
}

std::vector<std::shared_ptr<ReachNetwork::Child>> ReachNetwork::getChildren(int index) {
    std::vector<std::shared_ptr<ReachNetwork::Child>> result;
    for (const auto& edge : m_upstreamEdges) {
        if (edge->getParentIndex() == index) {
            auto node = m_nodes[edge->getChildIndex()];
            result.emplace_back(std::make_shared<Child>(Upstream, edge->getIntersection(), node));
        }
    }
    for (const auto& edge : m_downstreamEdges) {
        if (edge->getParentIndex() == index) {
            auto node = m_nodes[edge->getChildIndex()];
            result.emplace_back(std::make_shared<Child>(Downstream, edge->getIntersection(), node));
        }
    }
    return result;
}

std::vector<std::shared_ptr<ReachNetwork::Child>> ReachNetwork::Node::getChildren() const {
    return m_network->getChildren(m_reach->getIndex());
}

std::shared_ptr<ReachNetwork> ReachNetwork::filter(const std::function<bool(const std::shared_ptr<Node> &)>& predicate) {
    auto filteredNetwork = std::shared_ptr<ReachNetwork>(new ReachNetwork());
    for (const auto &node: m_nodes | std::views::values) {
        if (predicate(node)) {
            filteredNetwork->createNode(node->getReach());
        }
    }
    for (const auto& edge : m_upstreamEdges) {
        if (filteredNetwork->m_nodes.contains(edge->getParentIndex()) &&
                filteredNetwork->m_nodes.contains(edge->getChildIndex())) {
            filteredNetwork->addEdge(Upstream, edge->getParentIndex(), edge->getChildIndex(), edge->getIntersection());
        }
    }
    for (const auto& edge : m_downstreamEdges) {
        if (filteredNetwork->m_nodes.contains(edge->getParentIndex()) &&
                filteredNetwork->m_nodes.contains(edge->getChildIndex())) {
            filteredNetwork->addEdge(Downstream, edge->getParentIndex(), edge->getChildIndex(), edge->getIntersection());
        }
    }
    return filteredNetwork;
}
