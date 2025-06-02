#include <algorithm>
#include <queue>
#include <ranges>
#include <set>

#include "ReachNetwork.h"

#include <cassert>

ReachNetwork::Node::Node(
    const std::shared_ptr<ReachNetwork>& network,
    const std::shared_ptr<Reach>& reach
) {
    m_network = network;
    m_depth = 0;
    m_reach = reach;
    m_flowDirection = Unknown;
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
        m_upstreamEdges.push_back(std::make_shared<Edge>(parentIndex, childIndex, intersection));
    } else {
        m_downstreamEdges.push_back(std::make_shared<Edge>(parentIndex, childIndex, intersection));
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

/**
 * @brief Retrieves the child nodes of this node in the reach network.
 *
 * This function queries the associated ReachNetwork instance to retrieve
 * the child nodes connected to the current node. The child nodes are
 * returned as a vector of shared pointers to ReachNetwork::Child objects.
 *
 * @return A vector of shared pointers to the child nodes of this node.
 */
std::vector<std::shared_ptr<ReachNetwork::Child>> ReachNetwork::Node::getChildren() const {
    return m_network->getChildren(m_reach->getIndex());
}

void ReachNetwork::Node::updateFlowDirection() {
    if (m_depth == 0) {
        m_flowDirection = Backward;
        return;
    }
    auto up = getUpstreamParent();
    auto down = getDownstreamParent();
    // Constraint on both ends.
    if (up != nullptr && up->getNode()->m_flowDirection <= 1 && down != nullptr && down->getNode()->m_flowDirection <= 1) {
        const auto self = m_network->m_nodes[getReach()->getIndex()];
        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, DepthComparer> q;
        std::set<std::shared_ptr<Node>> visited;
        q.push(self);
        visited.insert(self);
        while (!q.empty()) {
            auto n = q.top();
            q.pop();
            // We have reached a root, but the queue is not empty. -> Multiple roots!
            if (n->getDepth() == 0 && !q.empty()) {
                findReasonableFlowDirection();
                return;
            }
            // This happens when the path in the network merges into one node "n".
            if (n != self && q.empty()) {
                // Compute min(start).
                auto current = up->getNode();
                int index = getReach()->getFront().index;
                while (current != n) {
                    assert(current->m_flowDirection == Forward || current->m_flowDirection == Backward);
                    if (current->m_flowDirection == Forward) {
                        index = current->m_reach->getFront().index;
                        current = current->getUpstreamParent()->getNode();
                    } else {
                        index = current->m_reach->getBack().index;
                        current = current->getDownstreamParent()->getNode();
                    }
                }
                int startMin = n->getReach()->findIndexedPoint(index);
                // Compute max(start).
                current = up->getNode();
                index = getReach()->getFront().index;
                while (current != n) {
                    assert(current->m_flowDirection == Forward || current->m_flowDirection == Backward);
                    if (current->m_flowDirection == Forward) {
                        index = current->m_reach->getBack().index;
                        current = current->getDownstreamParent()->getNode();
                    } else {
                        index = current->m_reach->getFront().index;
                        current = current->getUpstreamParent()->getNode();
                    }
                }
                int startMax = n->getReach()->findIndexedPoint(index);
                // Compute min(end).
                current = down->getNode();
                index = getReach()->getBack().index;
                while (current != n) {
                    assert(current->m_flowDirection == Forward || current->m_flowDirection == Backward);
                    if (current->m_flowDirection == Forward) {
                        index = current->m_reach->getFront().index;
                        current = current->getUpstreamParent()->getNode();
                    } else {
                        index = current->m_reach->getBack().index;
                        current = current->getDownstreamParent()->getNode();
                    }
                }
                int endMin = n->getReach()->findIndexedPoint(index);
                // Compute max(end).
                current = down->getNode();
                index = getReach()->getBack().index;
                while (current != n) {
                    assert(current->m_flowDirection == Forward || current->m_flowDirection == Backward);
                    if (current->m_flowDirection == Forward) {
                        index = current->m_reach->getBack().index;
                        current = current->getDownstreamParent()->getNode();
                    } else {
                        index = current->m_reach->getFront().index;
                        current = current->getUpstreamParent()->getNode();
                    }
                }
                int endMax = n->getReach()->findIndexedPoint(index);
                if (startMax <= endMin) {
                    m_flowDirection = n->m_flowDirection;
                    return;
                }
                if (endMax <= startMin) {
                    m_flowDirection = (FlowDirection)(1 - n->m_flowDirection);
                    return;
                }
                findReasonableFlowDirection();
                return;
            }
            if (n->getUpstreamParent() != nullptr && !visited.contains(n->getUpstreamParent()->getNode())) {
                q.push(n->getUpstreamParent()->getNode());
                visited.insert(n->getUpstreamParent()->getNode());
            }
            if (n->getDownstreamParent() != nullptr && !visited.contains(n->getDownstreamParent()->getNode())) {
                q.push(n->getDownstreamParent()->getNode());
                visited.insert(n->getDownstreamParent()->getNode());
            }
        }
    }
    findReasonableFlowDirection();
}

void ReachNetwork::Node::findReasonableFlowDirection() {
    const auto vec = m_reach->getBack().location - m_reach->getFront().location;
    if (std::abs(vec.x) >= std::abs(vec.y)) {
        m_flowDirection = vec.x > 0 ? LikelyForward : LikelyBackward;
    } else {
        m_flowDirection = vec.y > 0 ? LikelyForward : LikelyBackward;
    }
}

std::shared_ptr<ReachNetwork> ReachNetwork::filter(const std::function<bool(const std::shared_ptr<Node> &)>& predicate) {
    auto filteredNetwork = std::shared_ptr<ReachNetwork>(new ReachNetwork());
    filteredNetwork->m_self = filteredNetwork;
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
    for (const auto &[_, node] : filteredNetwork->m_nodes) {
        node->updateFlowDirection();
    }
    return filteredNetwork;
}

std::vector<Point> ReachNetwork::getReachPath(int reachIndex) {
    const auto getSourceParent = [](const std::shared_ptr<Node>& n) {
        return n->m_flowDirection % 2 == 0
            ? n->getUpstreamParent()
            : n->getDownstreamParent();
    };
    const auto getTargetParent = [](const std::shared_ptr<Node>& n) {
        return n->m_flowDirection % 2 == 0
            ? n->getDownstreamParent()
            : n->getUpstreamParent();
    };
    std::vector<Point> v;
    auto node = m_nodes[reachIndex];
    auto points = node->getReach()->getPoints();
    v.insert(v.begin(), points.begin(), points.end());
    if (node->m_flowDirection % 2 != 0) {
        std::ranges::reverse(v);
    }
    auto source = node, target = node;
    auto fp = getSourceParent(source);
    while (fp != nullptr) {
        const auto fpn = fp->getNode();
        const auto ipIndex = fpn->getReach()->findIndexedPoint(fp->getIntersection().index);
        if (fpn->m_flowDirection % 2 == 0) {
            v.insert(v.begin(), fpn->getReach()->getPoints().begin(), fpn->getReach()->getPoints().begin() + ipIndex);
        } else {
            std::vector tmp(fpn->getReach()->getPoints().begin() + ipIndex, fpn->getReach()->getPoints().end());
            std::ranges::reverse(tmp);
            v.insert(v.begin(), tmp.begin(), tmp.end());
        }
        source = fp->getNode();
        fp = getSourceParent(source);
    }
    auto tp = getTargetParent(target);
    while (tp != nullptr) {
        const auto tpn = tp->getNode();
        const auto ipIndex = tpn->getReach()->findIndexedPoint(tp->getIntersection().index);
        if (tpn->m_flowDirection % 2 == 0) {
            v.insert(v.end(), tpn->getReach()->getPoints().begin() + ipIndex, tpn->getReach()->getPoints().end());
        } else {
            std::vector tmp(tpn->getReach()->getPoints().begin(), tpn->getReach()->getPoints().begin() + ipIndex);
            std::ranges::reverse(tmp);
            v.insert(v.end(), tmp.begin(), tmp.end());
        }
        target = tp->getNode();
        tp = getTargetParent(target);
    }
    if (node->m_flowDirection % 2 != 0) {
        std::ranges::reverse(v);
    }
    return v;
}
