#include <ranges>

#include "ReachNetworkDisplayFrame.h"

ReachNetworkDisplayFrame::ReachNetworkDisplayFrame(const QString& name, const std::shared_ptr<ReachNetwork>& network) {
    m_name = name;
    m_network = network;
}

QGraphicsScene* ReachNetworkDisplayFrame::getScene(int maxDepth, double minDelta) const {
    auto bounds = m_network->getBounds();
    auto width = bounds.second.x - bounds.first.x;
    auto height = bounds.second.y - bounds.first.y;
    auto scene = new QGraphicsScene(bounds.first.x, bounds.first.y,
                                 width, height);
    scene->setBackgroundBrush(QBrush(Qt::black));
    for (const auto &[_, node]: m_network->getNodes()) {
        auto depth = node->getDepth();
        if (depth > maxDepth || node->getReach()->getDelta() < minDelta) continue;
        auto path = createPath(node->getReach()->getPoints());
        if (path.elementCount() > 1) {
            scene->addPath(path, brushes[depth % brushes.size()], QBrush(Qt::transparent));
        }
    }
    return scene;
}