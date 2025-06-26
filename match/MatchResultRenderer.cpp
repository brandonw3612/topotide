#include "MatchResultRenderer.h"

#include <QVBoxLayout>

MatchResultRenderer::MatchResultRenderer() {
    auto rootLayout = new QVBoxLayout;
    m_topGraphicsView = new SGraphicsView(0);
    m_middleGraphicsView = new SGraphicsView(1);
    m_bottomGraphicsView = new SGraphicsView(2);
    rootLayout->addWidget(m_topGraphicsView, 1);
    rootLayout->addWidget(m_middleGraphicsView, 1);
    rootLayout->addWidget(m_bottomGraphicsView, 1);

    m_topGraphicsView->setSynchronizedView(m_middleGraphicsView);
    m_middleGraphicsView->setSynchronizedView(m_topGraphicsView);

    this->setLayout(rootLayout);
}

void MatchResultRenderer::setFrame(const std::shared_ptr<PrecomputedDisplayFrame> &frame) {
    if (m_frame == frame) {
        return; // No change
    }

    if (m_frame != nullptr) {
        disconnect(m_topGraphicsView, &SGraphicsView::onPointerMoved, m_frame.get(), &PrecomputedDisplayFrame::updatePointerLocation);
        disconnect(m_frame.get(), &PrecomputedDisplayFrame::onViewUpdated, this, &MatchResultRenderer::refreshViews);
    }

    m_frame = frame;

    connect(m_topGraphicsView, &SGraphicsView::onPointerMoved, m_frame.get(), &PrecomputedDisplayFrame::updatePointerLocation);
    connect(m_frame.get(), &PrecomputedDisplayFrame::onViewUpdated, this, &MatchResultRenderer::refreshViews);

    refreshViews();
}

void MatchResultRenderer::refreshViews() {
    m_topGraphicsView->setScene(m_frame->getScene());
    m_middleGraphicsView->setScene(m_frame->getMappedScene());
    m_bottomGraphicsView->setScene(m_frame->getOriginalMappedScene());
}
