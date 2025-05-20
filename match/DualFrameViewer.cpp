#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include "DualFrameViewer.h"

#include <QPushButton>

#include "NetworkConverter.h"

DualFrameViewer::DualFrameViewer() {
    setWindowTitle("Dual Frame Viewer");

    auto rootLayout = new QVBoxLayout;

    auto topCommandLayout = new QHBoxLayout;
    auto depthLabel = new QLabel("Max node depth");
    depthLabel->setStyleSheet("QLabel { font-size: 14px; }");
    m_depthSpinBox = new QSpinBox;
    m_depthSpinBox->setValue(2);
    m_depthSpinBox->setRange(0, 10);
    m_depthSpinBox->setSingleStep(1);
    m_depthSpinBox->setMinimumWidth(100);
    m_depthSpinBox->setMaximumWidth(200);
    connect(m_depthSpinBox, &QSpinBox::valueChanged, this, &DualFrameViewer::onFilterChanged);
    auto deltaLabel = new QLabel("Min delta");
    deltaLabel->setStyleSheet("QLabel { font-size: 14px; }");
    m_deltaLineEdit = new QLineEdit;
    m_deltaLineEdit->setText("100");
    m_deltaLineEdit->setMinimumWidth(100);
    m_deltaLineEdit->setMaximumWidth(200);
    connect(m_deltaLineEdit, &QLineEdit::textChanged, this, &DualFrameViewer::onFilterChanged);
    topCommandLayout->addStretch();
    topCommandLayout->addWidget(depthLabel);
    topCommandLayout->addWidget(m_depthSpinBox);
    topCommandLayout->addSpacing(20);
    topCommandLayout->addWidget(deltaLabel);
    topCommandLayout->addWidget(m_deltaLineEdit);

    auto topViewLayout = createSingleView(1, "Frame #1", m_topViewComboBox, m_topGraphicsView);
    auto bottomViewLayout = createSingleView(2, "Frame #2", m_bottomViewComboBox, m_bottomGraphicsView);

    m_topGraphicsView->setSynchronizedView(m_bottomGraphicsView);
    m_bottomGraphicsView->setSynchronizedView(m_topGraphicsView);

    auto midLayout = new QHBoxLayout;
    auto button = new QPushButton("Match ReachPath ⬇️");
    connect(button, &QPushButton::clicked, this, &DualFrameViewer::onMatchReachPath);
    midLayout->addStretch();
    midLayout->addWidget(button);
    midLayout->addStretch();

    rootLayout->addLayout(topCommandLayout);
    rootLayout->addLayout(topViewLayout);
    rootLayout->addLayout(midLayout);
    rootLayout->addLayout(bottomViewLayout);

    this->setLayout(rootLayout);
    resize(1000, 800);
}

QVBoxLayout* DualFrameViewer::createSingleView(int id, const QString& title, QComboBox* &comboBox, SGraphicsView* &graphicsView) {
    auto layout = new QVBoxLayout;

    auto label = new QLabel(title);
    label->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
    layout->addWidget(label);

    comboBox = new QComboBox;
    comboBox->setMaximumWidth(400);
    comboBox->setProperty("ID", QVariant(id));
    connect(comboBox, &QComboBox::currentIndexChanged, this, &DualFrameViewer::onComboBoxChanged);
    layout->addWidget(comboBox);

    graphicsView = new SGraphicsView(id);
    layout->addWidget(graphicsView, 1);

    return layout;

}

void DualFrameViewer::onComboBoxChanged() {
    auto comboBox = qobject_cast<QComboBox*>(sender());
    auto frame = comboBox->currentData().value<std::shared_ptr<NetworkDisplayFrame>>();
    auto view = comboBox->property("ID").toInt() == 1 ? m_topGraphicsView : m_bottomGraphicsView;
    auto oldFrame = comboBox->property("ID").toInt() == 1 ? m_topFrame : m_bottomFrame;
    if (oldFrame != nullptr) {
        disconnect(view, &SGraphicsView::onPointerMoved, oldFrame.get(), &NetworkDisplayFrame::updatePointerLocation);
        disconnect(view, &SGraphicsView::onPointerPressed, oldFrame.get(), &NetworkDisplayFrame::highlightReachPath);
        disconnect(oldFrame.get(), &NetworkDisplayFrame::onViewUpdated, this, &DualFrameViewer::refreshViews);
    }
    if (frame == nullptr) view->setScene(nullptr);
    else {
        frame->index(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat());
        view->setScene(frame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
        connect(view, &SGraphicsView::onPointerMoved, frame.get(), &NetworkDisplayFrame::updatePointerLocation);
        connect(view, &SGraphicsView::onPointerPressed, frame.get(), &NetworkDisplayFrame::highlightReachPath);
        connect(frame.get(), &NetworkDisplayFrame::onViewUpdated, this, &DualFrameViewer::refreshViews);
    }
    if (comboBox->property("ID").toInt() == 1) {
        m_topFrame = frame;
    } else {
        m_bottomFrame = frame;
    }
}

void DualFrameViewer::setFrames(const std::vector<std::shared_ptr<NetworkDisplayFrame>> &frames) {
    m_topViewComboBox->clear();
    m_bottomViewComboBox->clear();
    m_topViewComboBox->addItem("Select", QVariant::fromValue<std::shared_ptr<NetworkDisplayFrame>>(nullptr));
    m_bottomViewComboBox->addItem("Select", QVariant::fromValue<std::shared_ptr<NetworkDisplayFrame>>(nullptr));
    for (const auto &frame: frames) {
        m_topViewComboBox->addItem(frame->getName(), QVariant::fromValue(frame));
        m_bottomViewComboBox->addItem(frame->getName(), QVariant::fromValue(frame));
    }
}

void DualFrameViewer::onFilterChanged() {
    if (m_topFrame != nullptr) {
        m_topFrame->index(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat());
        m_topGraphicsView->setScene(m_topFrame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
    }
    if (m_bottomFrame != nullptr) {
        m_bottomFrame->index(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat());
        m_bottomGraphicsView->setScene(m_bottomFrame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
    }
}

void DualFrameViewer::onMatchReachPath() {
    auto filtered = m_bottomFrame->getNetwork()->filter(
            [this](const std::shared_ptr<ReachNetwork::Node>& node) {
                return node->getDepth() <= m_depthSpinBox->value() && node->getReach()->getDelta() >= m_deltaLineEdit->text().toFloat();
            });
    m_bottomFrame->matchReachPathFrom(m_topFrame, NetworkConverter::rn2ng(filtered), 10.0);
}

void DualFrameViewer::refreshViews() {
    if (m_topFrame != nullptr) {
        m_topGraphicsView->setScene(m_topFrame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
    }
    if (m_bottomFrame != nullptr) {
        m_bottomGraphicsView->setScene(m_bottomFrame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
    }
}
