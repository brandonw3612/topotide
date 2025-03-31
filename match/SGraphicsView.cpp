#include "SGraphicsView.h"
#include <QScrollBar>

SGraphicsView::SGraphicsView(int id) {
    m_isPanning = false;
    m_synchronizedView = nullptr;
    setRenderHint(QPainter::Antialiasing);
}

void SGraphicsView::setSynchronizedView(SGraphicsView *view) {
    m_synchronizedView = view;
}

void SGraphicsView::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.1;
    int delta = event->angleDelta().y();
    if (delta == 0) return;
    if (delta > 0) {
        if (transform().m11() > 5) return;
        setTransform(transform().scale(scaleFactor, scaleFactor));
        if (m_synchronizedView != nullptr) m_synchronizedView->scale(scaleFactor, scaleFactor);
    } else {
        if (transform().m11() < 0.5) return;
        setTransform(transform().scale(1 / scaleFactor, 1 / scaleFactor));
        if (m_synchronizedView != nullptr) m_synchronizedView->scale(1 / scaleFactor, 1 / scaleFactor);
    }
}

void SGraphicsView::mousePressEvent(QMouseEvent *event) {
    m_isPanning = true;
    m_activePosition = event->pos();
    setCursor(Qt::ClosedHandCursor);
}

void SGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        QPointF delta = event->pos() - m_activePosition;
        m_activePosition = event->pos();
        auto newX = horizontalScrollBar()->value() - delta.x();
        auto newY = verticalScrollBar()->value() - delta.y();
        horizontalScrollBar()->setValue((int) newX);
        verticalScrollBar()->setValue((int) newY);
        if (m_synchronizedView != nullptr) {
            m_synchronizedView->horizontalScrollBar()->setValue((int) newX);
            m_synchronizedView->verticalScrollBar()->setValue((int) newY);
        }
    }
}

void SGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    m_isPanning = false;
    setCursor(Qt::ArrowCursor);
}
