#ifndef PRECOMPUTEDDISPLAYFRAME_H
#define PRECOMPUTEDDISPLAYFRAME_H

#include "Frame.h"
#include "PointSearchTree.h"
#include "PreComputedReachNetwork.h"

class PrecomputedDisplayFrame : public QObject, public Frame {

    Q_OBJECT

private:
    /// Name of the reach network.
    QString m_name;
    /// The reach network to be displayed.
    std::shared_ptr<PreComputedReachNetwork> m_network;
    std::shared_ptr<PointSearchTree> m_pointSearchTree;
    int m_highlightedReachId;
    int m_depth;

public:
    explicit PrecomputedDisplayFrame(const QString& name, const std::shared_ptr<PreComputedReachNetwork>& network);

    [[nodiscard]] const QString& getName() const override { return m_name; }
    [[nodiscard]] const std::shared_ptr<PreComputedReachNetwork>& getNetwork() const { return m_network; }

    [[nodiscard]] QGraphicsScene* getScene() const;
    [[nodiscard]] QGraphicsScene* getMappedScene() const;
    [[nodiscard]] QGraphicsScene* getScene(int maxDepth, double minDelta) const override { return getScene(); }
    void updatePointerLocation(double x, double y);

private:
    static QPen createPen(float alpha, double width);

signals:
    void onViewUpdated();
};



#endif //PRECOMPUTEDDISPLAYFRAME_H
