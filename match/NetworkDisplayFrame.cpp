#include "NetworkDisplayFrame.h"

#include <iostream>

#include "NetworkConverter.h"
#include "PathMatcher.h"

NetworkDisplayFrame::NetworkDisplayFrame(const QString &name, const std::shared_ptr<ReachNetwork> &reachNetwork) {
    m_name = name;
    m_reachNetwork = reachNetwork;
    std::cout << "Indexing " << name.toStdString() << "..." << std::endl;
    m_highlightedReachId = -1;
}

void NetworkDisplayFrame::matchReachPathFrom(const std::shared_ptr<NetworkDisplayFrame> &otherFrame, const std::shared_ptr<NetworkGraph>& filteredNetwork, double th) {
    if (otherFrame->m_reachPath.empty()) return;
    auto matchedPath = PathMatcher::match(otherFrame->m_reachPath, filteredNetwork, th);
    std::cout << "Matched a path with len " << matchedPath.size() << std::endl;
    m_reachPath = matchedPath;
    emit onViewUpdated();
}

void NetworkDisplayFrame::updatePointerLocation(double x, double y) {
    auto reachId = m_pointSearchTree->findReach(x, y);
    if (reachId != m_highlightedReachId) {
        m_highlightedReachId = reachId;
        emit onViewUpdated();
    }
}

void NetworkDisplayFrame::highlightReachPath() {
    if (m_highlightedReachId != -1) {
        auto reachPath = m_reachNetwork->getReachPath(m_highlightedReachId);
        m_reachPath = reachPath;
        emit onViewUpdated();
    } else {
        m_reachPath.clear();
        emit onViewUpdated();
    }
}

void NetworkDisplayFrame::index(int maxDepth, double minDelta) {
    m_pointSearchTree = std::make_shared<PointSearchTree>(m_reachNetwork, maxDepth, minDelta);
}

QGraphicsScene * NetworkDisplayFrame::getScene(int maxDepth, double minDelta) const {
    auto bounds = m_reachNetwork->getBounds();
    auto width = bounds.second.x - bounds.first.x;
    auto height = bounds.second.y - bounds.first.y;
    auto scene = new QGraphicsScene(bounds.first.x, bounds.first.y,
                                 width, height);
    scene->setBackgroundBrush(QBrush(Qt::black));
    auto reachPathPath = createPath(m_reachPath);
    scene->addPath(reachPathPath, QPen(QBrush(QColor(Qt::white)), 5), QBrush(Qt::transparent));
    for (const auto& [_, node]: m_reachNetwork->getNodes()) {
        auto depth = node->getDepth();
        if (depth > maxDepth || node->getReach()->getDelta() < minDelta) continue;
        auto path = createPath(node->getReach()->getPoints());
        if (path.elementCount() > 1) {
            if (m_highlightedReachId == node->getReach()->getIndex()) {
                scene->addPath(path, highlightedBrushes[depth % highlightedBrushes.size()], QBrush(Qt::transparent));
            } else {
                scene->addPath(path, brushes[depth % brushes.size()], QBrush(Qt::transparent));
            }
        }
    }
    return scene;
}
