#include "SGraphicsView.h"
#include <iostream>

SGraphicsView::SGraphicsView(int id) {
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void SGraphicsView::setSynchronizedView(SGraphicsView *view) {
    m_synchronizedView = view;
}

void SGraphicsView::wheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y();
    if (delta == 0) return;
    if (delta > 0) {
        if (transform().m11() > 5) return;
        setTransform(transform().scale(1.05, 1.05));
        if (m_synchronizedView != nullptr) m_synchronizedView->scale(1.05, 1.05);
    } else {
        if (transform().m11() < 0.5) return;
        setTransform(transform().scale(0.95, 0.95));
        if (m_synchronizedView != nullptr) m_synchronizedView->scale(0.95, 0.95);
    }
}

void SGraphicsView::mousePressEvent(QMouseEvent *event) {
    m_activePosition = event->pos();
}

void SGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QPointF delta = (mapToScene(event->pos()) - mapToScene(m_activePosition));
        std::cout << delta.x() << " " << delta.y() << std::endl;
        setSceneRect(sceneRect().translated(-delta.x(), -delta.y()));
        m_synchronizedView->setSceneRect(sceneRect().translated(-delta.x(), -delta.y()));
        m_activePosition = event->pos();
    }
}
