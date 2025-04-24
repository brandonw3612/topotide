#include "AbstractFrame.h"
#include <queue>
#include <QPainterPath>
#include <QGraphicsWidget>

AbstractFrame::AbstractFrame(const QString& name, const std::shared_ptr<AbstractGraph>& graph) {
    m_name = name;
    m_graph = graph;
}

QGraphicsScene *AbstractFrame::getScene(int maxDepth, double minDelta) const {
    auto bounds = m_graph->getBounds();
    auto width = bounds.second.x - bounds.first.x;
    auto height = bounds.second.y - bounds.first.y;
    auto scene = new QGraphicsScene(bounds.first.x, bounds.first.y,
                                 width, height);
    scene->setBackgroundBrush(QBrush(Qt::black));
    std::map<std::shared_ptr<AbstractGraph::AGNode>, int> depthMap;
    for (const auto &node: m_graph->getNodes()) {
        int depth = 0;
        if (node->getSplittingParent() != nullptr) {
            depth = std::max(depth, depthMap[node->getSplittingParent()] + 1);
        }
        if (node->getMergingParent() != nullptr) {
            depth = std::max(depth, depthMap[node->getMergingParent()] + 1);
        }
        depthMap[node] = depth;
        if (depth > maxDepth || node->getChannel()->getDelta() < minDelta) continue;
        auto path = createPath(node->getChannel()->getPoints());
        if (path.elementCount() > 1) {
            scene->addPath(path, brushes[depth % brushes.size()], QBrush(Qt::transparent));
        }
    }
    return scene;
}
