#ifndef TOPOTIDE_FRAME_H
#define TOPOTIDE_FRAME_H

#include <vector>
#include <QGraphicsScene>
#include <QPainterPath>
#include "point.h"

class Frame {
public:
    virtual ~Frame() = default;

    virtual QGraphicsScene* getScene(int maxDepth, double minDelta) const = 0;
    virtual const QString& getName() const = 0;

protected:
    static QPainterPath createPath(const std::vector<Point>& points);
};

#endif // FRAME_H