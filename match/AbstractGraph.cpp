#include "AbstractGraph.h"

std::pair<Point, Point> AbstractGraph::getBounds() const {
    auto bounds = m_nodes[0]->getChannel()->getBounds();
    for (const auto &n : m_nodes) {
        auto b = n->getChannel()->getBounds();
        if (b.first.x < bounds.first.x) bounds.first.x = b.first.x;
        if (b.first.y < bounds.first.y) bounds.first.y = b.first.y;
        if (b.second.x > bounds.second.x) bounds.second.x = b.second.x;
        if (b.second.y > bounds.second.y) bounds.second.y = b.second.y;
    }
    return bounds;
}

std::shared_ptr<AbstractChannel> AbstractGraph::getParentlessChannel(double minDelta) const {
    for (auto &n : m_nodes) {
        if (n->getMergingParent() != nullptr || n->getSplittingParent() != nullptr) continue;
        if (n->getChannel()->getDelta() < minDelta) continue;
        return n->getChannel();
    }
    return nullptr;
}