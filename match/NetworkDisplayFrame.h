#ifndef NETWORKDISPLAYFRAME_H
#define NETWORKDISPLAYFRAME_H

#include <memory>
#include <QGraphicsScene>
#include <QObject>
#include <QPen>

#include "Frame.h"
#include "networkgraph.h"
#include "PointSearchTree.h"
#include "ReachNetwork.h"

class NetworkDisplayFrame : public QObject, public Frame {

    Q_OBJECT

private:
    QString m_name;
    std::shared_ptr<ReachNetwork> m_reachNetwork;
    std::shared_ptr<PointSearchTree> m_pointSearchTree;
    std::vector<Point> m_reachPath;
    std::vector<Point> m_reachSegment;
    int m_highlightedReachId;

public:
    NetworkDisplayFrame(const QString& name, const std::shared_ptr<ReachNetwork>& reachNetwork);
    [[nodiscard]] const QString& getName() const override { return m_name; }
    [[nodiscard]] const std::shared_ptr<ReachNetwork>& getNetwork() const { return m_reachNetwork; }

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

    const std::vector<QPen> highlightedBrushes = {
        QPen(QBrush(QColor::fromHsv(0, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(210, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(60, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(270, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(120, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(330, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(180, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(30, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(240, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(90, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(300, 255, 255)), 3),
        QPen(QBrush(QColor::fromHsv(150, 255, 255)), 3)
    };

public:
    void matchReachPathFrom(const std::shared_ptr<NetworkDisplayFrame>& otherFrame, const std::shared_ptr<NetworkGraph>& filteredNetwork, double th);
    void updatePointerLocation(double x, double y);
    void highlightReachPath();
    void index(int maxDepth, double minDelta);
    [[nodiscard]] QGraphicsScene* getScene(int maxDepth, double minDelta) const override;

signals:
    void onViewUpdated();
};



#endif //NETWORKDISPLAYFRAME_H
