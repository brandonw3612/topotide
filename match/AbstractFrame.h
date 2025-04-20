#ifndef TOPOTIDE_ABSTRACTFRAME_H
#define TOPOTIDE_ABSTRACTFRAME_H

#include <QString>
#include <QGraphicsScene>
#include "AbstractGraph.h"
#include "Frame.h"

// TODO: It is a bit weird that AbstractFrame inherits from Frame, which is an abstract class.
// This could be combined with renaming AbstractChannel and AbstractGraph since these are also not abstract classes.
class AbstractFrame : public Frame {
private:
    QString m_name;
    std::shared_ptr<AbstractGraph> m_graph;

public:
    explicit AbstractFrame(const QString& name, const std::shared_ptr<AbstractGraph>& graph);

    [[nodiscard]] const QString& getName() const override { return m_name; }
    [[nodiscard]] const std::shared_ptr<AbstractGraph>& getGraph() const { return m_graph; }

public:
    [[nodiscard]] QGraphicsScene* getScene(int maxDepth, double minDelta) const override;

private:
    const std::vector<QPen> brushes = {
        QPen(QBrush(QColor::fromHsv(0, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(210, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(60, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(270, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(120, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(330, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(180, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(30, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(240, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(90, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(300, 255, 255)), 1),
        QPen(QBrush(QColor::fromHsv(150, 255, 255)), 1)
    };
};

#endif //TOPOTIDE_ABSTRACTFRAME_H
