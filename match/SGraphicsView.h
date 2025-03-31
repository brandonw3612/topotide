#ifndef TOPOTIDE_SGRAPHICSVIEW_H
#define TOPOTIDE_SGRAPHICSVIEW_H

#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsView>

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
};


#endif //TOPOTIDE_SGRAPHICSVIEW_H
