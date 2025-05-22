#include "MatchResultRenderer.h"

#include <QVBoxLayout>

MatchResultRenderer::MatchResultRenderer(const std::shared_ptr<PrecomputedDisplayFrame> &frame) {
    m_frame = frame;

    setWindowTitle("Map Result Viewer");

    auto rootLayout = new QVBoxLayout;
    m_topGraphicsView = new SGraphicsView(0);
    m_bottomGraphicsView = new SGraphicsView(1);
    rootLayout->addWidget(m_topGraphicsView, 1);
    rootLayout->addWidget(m_bottomGraphicsView, 1);

    m_topGraphicsView->setSynchronizedView(m_bottomGraphicsView);
    m_bottomGraphicsView->setSynchronizedView(m_topGraphicsView);

    connect(m_topGraphicsView, &SGraphicsView::onPointerMoved, m_frame.get(), &PrecomputedDisplayFrame::updatePointerLocation);
    connect(m_frame.get(), &PrecomputedDisplayFrame::onViewUpdated, this, &MatchResultRenderer::refreshViews);

    this->setLayout(rootLayout);

    refreshViews();
}

void MatchResultRenderer::refreshViews() {
    m_topGraphicsView->setScene(m_frame->getScene());
    m_bottomGraphicsView->setScene(m_frame->getMappedScene());
}
