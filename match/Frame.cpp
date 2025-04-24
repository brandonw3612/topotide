#include "Frame.h"

QPainterPath Frame::createPath(const std::vector<Point> &points) {
    QPainterPath path;
    int index = 0;
    while (index < points.size() && !std::isfinite(points[index].h)) index++;
    if (index == points.size()) return path;
    path.moveTo(points[index].x, points[index].y);
    index++;
    while (index < points.size()) {
        if (std::isfinite(points[index].h)) {
            path.lineTo(points[index].x, points[index].y);
        }
        index++;
    }
    return path;
}