#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include "DualFrameViewer.h"

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

    rootLayout->addLayout(topCommandLayout);
    rootLayout->addLayout(topViewLayout);
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
    auto frame = comboBox->currentData().value<std::shared_ptr<Frame>>();
    auto view = comboBox->property("ID").toInt() == 1 ? m_topGraphicsView : m_bottomGraphicsView;
    if (frame == nullptr) view->setScene(nullptr);
    else view->setScene(frame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
}

void DualFrameViewer::setFrames(const std::vector<std::shared_ptr<Frame>> &frames) {
    m_topViewComboBox->clear();
    m_bottomViewComboBox->clear();
    m_topViewComboBox->addItem("Select", QVariant::fromValue<std::shared_ptr<Frame>>(nullptr));
    m_bottomViewComboBox->addItem("Select", QVariant::fromValue<std::shared_ptr<Frame>>(nullptr));
    for (const auto &frame: frames) {
        m_topViewComboBox->addItem(frame->getName(), QVariant::fromValue(frame));
        m_bottomViewComboBox->addItem(frame->getName(), QVariant::fromValue(frame));
    }
}

void DualFrameViewer::onFilterChanged() {
    auto topFrame = m_topViewComboBox->currentData().value<std::shared_ptr<Frame>>();
    if (topFrame != nullptr) {
        m_topGraphicsView->setScene(topFrame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
    }
    auto bottomFrame = m_bottomViewComboBox->currentData().value<std::shared_ptr<Frame>>();
    if (bottomFrame != nullptr) {
        m_bottomGraphicsView->setScene(bottomFrame->getScene(m_depthSpinBox->value(), m_deltaLineEdit->text().toFloat()));
    }
}
