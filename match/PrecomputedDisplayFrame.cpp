#include "PrecomputedDisplayFrame.h"

PrecomputedDisplayFrame::PrecomputedDisplayFrame(const QString &name,
    const std::shared_ptr<PreComputedReachNetwork> &network) {
    m_name = name;
    m_network = network;
    m_depth = 0;
    m_pointSearchTree = std::make_shared<PointSearchTree>(m_network);
    for (int id : m_network->getMappedReachIndices()) {
        const auto d = m_network->getNodes()[id]->getDepth();
        if (d > m_depth) {
            m_depth = d;
        }
    }
    m_highlightedReachId = -1;
}

QGraphicsScene * PrecomputedDisplayFrame::getScene() const {
    auto bounds = m_network->getBounds();
    auto width = bounds.second.x - bounds.first.x;
    auto height = bounds.second.y - bounds.first.y;
    auto scene = new QGraphicsScene(bounds.first.x, bounds.first.y,
                                 width, height);
    for (const auto& id : m_network->getMappedReachIndices()) {
        auto node = m_network->getNodes()[id];
        auto depth = node->getDepth();
        auto path = createPath(node->getReach()->getPoints());
        if (path.elementCount() > 1) {
            scene->addPath(path, createPen(0.7 - 0.65 * depth / m_depth, 2 - 1.5 * depth / m_depth), QBrush(Qt::transparent));
        }
    }
    if (m_highlightedReachId != -1) {
        auto path = createPath(m_network->getNodes()[m_highlightedReachId]->getReach()->getPoints());
        scene->addPath(path, createPen(1, 3), QBrush(Qt::transparent));
    }
    for (const auto& p : m_network->getMinima()) {
        scene->addEllipse(p.x - 0.5, p.y - 0.5, 1.0, 1.0, createDotPen(1, 1), QBrush(Qt::transparent));
    }
    return scene;
}

QGraphicsScene * PrecomputedDisplayFrame::getMappedScene() const {
    auto bounds = m_network->getBounds();
    auto width = bounds.second.x - bounds.first.x;
    auto height = bounds.second.y - bounds.first.y;
    auto scene = new QGraphicsScene(bounds.first.x, bounds.first.y,
                                 width, height);
    for (const auto& id : m_network->getMappedReachIndices()) {
        auto node = m_network->getNodes()[id];
        auto depth = node->getDepth();
        auto path = createPath(m_network->getMatchedSegment(id));
        if (path.elementCount() > 1) {
            scene->addPath(path, createPen(0.7 - 0.65 * depth / m_depth, 2 - 1.5 * depth / m_depth), QBrush(Qt::transparent));
        }
    }
    if (m_highlightedReachId != -1) {
        auto path = createPath(m_network->getMatchedSegment(m_highlightedReachId));;
        scene->addPath(path, createPen(1, 3), QBrush(Qt::transparent));
    }
    for (const auto& id : m_network->getMappedReachIndices()) {
        auto node = m_network->getNodes()[id];
        auto depth = node->getDepth();
        auto p1 = m_network->getMatchedSegment(id).front();
        auto p2 = m_network->getMatchedSegment(id).back();
        for (const auto& p : {p1, p2}) {
            scene->addEllipse(p.x - 0.5, p.y - 0.5, 1.0, 1.0, createDotPen(1, 1), QBrush(Qt::transparent));
        }
    }
    return scene;
}

void PrecomputedDisplayFrame::updatePointerLocation(double x, double y) {
    auto reachId = m_pointSearchTree->findReach(x, y);
    if (reachId != m_highlightedReachId) {
        m_highlightedReachId = reachId;
        emit onViewUpdated();
    }
}

QPen PrecomputedDisplayFrame::createPen(float alpha, double width) {
    return QPen(QBrush(QColor::fromRgb(0, (int)(255 * alpha), 0)), width);
}

QPen PrecomputedDisplayFrame::createDotPen(float alpha, double width) {
    return QPen(QBrush(QColor::fromRgb((int)(255 * alpha), 0, 0)), width);
}
