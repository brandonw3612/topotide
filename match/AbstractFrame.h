#ifndef TOPOTIDE_ABSTRACTFRAME_H
#define TOPOTIDE_ABSTRACTFRAME_H

#include <QString>
#include <QGraphicsScene>
#include "AbstractGraph.h"

class AbstractFrame {
private:
    QString m_name;
    std::shared_ptr<AbstractGraph> m_graph;

public:
    explicit AbstractFrame(const QString& name, const std::shared_ptr<AbstractGraph>& graph);

    [[nodiscard]] const QString& getName() const { return m_name; }
    [[nodiscard]] const std::shared_ptr<AbstractGraph>& getGraph() const { return m_graph; }

public:
    [[nodiscard]] QGraphicsScene* getScene(int maxDepth, double minDelta) const;

private:
    static QPainterPath createPath(const std::vector<Point>& points);
    std::shared_ptr<AbstractChannel> m_matchedChannel;
    std::shared_ptr<AbstractChannel> m_mainChannel;
public:
    void setMatchedChannel(const std::shared_ptr<AbstractChannel>& channel){m_matchedChannel = channel;};
    void setMainChannel(const std::shared_ptr<AbstractChannel>& channel){m_mainChannel = channel;};
    std::shared_ptr<AbstractChannel> getMainChannel(){return m_mainChannel;};

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
    const QPen matchedBrush = QPen(QBrush(QColor::fromHsv(0, 0, 255)), 1);
};

#endif //TOPOTIDE_ABSTRACTFRAME_H
