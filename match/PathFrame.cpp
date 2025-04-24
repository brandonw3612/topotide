#include "PathFrame.h"

PathFrame::PathFrame(const QString &name, const std::vector<Point> &path, std::pair<Point, Point> bounds) {
    m_name = name;
    m_path = path;
    m_bounds = bounds;
}

QGraphicsScene *PathFrame::getScene(int maxDepth, double minDelta) const
{
    auto width = m_bounds.second.x - m_bounds.first.x;
    auto height = m_bounds.second.y - m_bounds.first.y;
    auto scene = new QGraphicsScene(m_bounds.first.x, m_bounds.first.y,
                                 width, height);
    scene->setBackgroundBrush(QBrush(Qt::black));
    scene->addPath(createPath(m_path), QPen(QBrush(QColor(Qt::white)), 1), QBrush(Qt::transparent));
    return scene;
}
