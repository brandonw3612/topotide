#ifndef TOPOTIDE_SGRAPHICSVIEW_H
#define TOPOTIDE_SGRAPHICSVIEW_H

#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsView>

#include "NetworkDisplayFrame.h"

class SGraphicsView : public QGraphicsView {

    Q_OBJECT

private:
    SGraphicsView* m_synchronizedView;
    bool m_isPanning;
    QPoint m_activePosition;

public:
    SGraphicsView(int id);

public:
    void setSynchronizedView(SGraphicsView* view);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void onPointerMoved(double x, double y);
    void onPointerPressed();
};


#endif //TOPOTIDE_SGRAPHICSVIEW_H
