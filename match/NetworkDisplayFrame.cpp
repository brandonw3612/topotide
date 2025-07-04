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
    auto match = PathMatcher::match(otherFrame->m_reachPath, filteredNetwork, th);
    auto matchedPath = match.path;
    auto matchedSegmentIndex = PathMatcher::matchSegment(otherFrame->m_reachPath, otherFrame->m_reachSegment, matchedPath);
    auto matchedSegment = std::vector(matchedPath.begin() + matchedSegmentIndex.first,
                                      matchedPath.begin() + matchedSegmentIndex.second + 1);
    std::cout << "Matched a path with len " << matchedPath.size() << " and the subsegment path with len " << matchedSegment.size() << std::endl;
    m_reachPath = matchedPath;
    m_reachSegment = matchedSegment;
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
        auto reachSegment = m_reachNetwork->getNodes()[m_highlightedReachId]->getReach()->getPoints();
        m_reachPath = reachPath;
        m_reachSegment = reachSegment;
        emit onViewUpdated();
    } else {
        m_reachPath.clear();
        m_reachSegment.clear();
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
    auto reachSegmentPath = createPath(m_reachSegment);
    scene->addPath(reachSegmentPath, QPen(QBrush(QColor(Qt::darkGray)), 6), QBrush(Qt::transparent));
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
