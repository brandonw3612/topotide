#ifndef REACHNETWORKDISPLAYFRAME_H
#define REACHNETWORKDISPLAYFRAME_H

#include "Frame.h"
#include "ReachNetwork.h"

/// Display frame for a reach network which prepares the data for visualization.
class ReachNetworkDisplayFrame : public Frame {
private:
    /// Name of the reach network.
    QString m_name;
    /// The reach network to be displayed.
    std::shared_ptr<ReachNetwork> m_network;

public:
    explicit ReachNetworkDisplayFrame(const QString& name, const std::shared_ptr<ReachNetwork>& network);

    [[nodiscard]] const QString& getName() const override { return m_name; }
    [[nodiscard]] const std::shared_ptr<ReachNetwork>& getNetwork() const { return m_network; }

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



#endif //REACHNETWORKDISPLAYFRAME_H
