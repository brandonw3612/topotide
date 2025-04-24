#ifndef TOPOTIDE_PATHFRAME_H
#define TOPOTIDE_PATHFRAME_H

#include <vector>
#include <QString>
#include <QGraphicsScene>
#include "Frame.h"
#include "point.h"

class PathFrame : public Frame {
private:
    QString m_name;
    std::vector<Point> m_path;
    std::pair<Point, Point> m_bounds;

public:
    explicit PathFrame(const QString& name, const std::vector<Point>& path, std::pair<Point, Point> bounds);

    [[nodiscard]] const QString& getName() const override { return m_name; }
    [[nodiscard]] QGraphicsScene* getScene(int maxDepth, double minDelta) const override;
};

#endif // PATHFRAME_H