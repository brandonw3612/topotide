#include <QVBoxLayout>
#include <QLabel>
#include "DualFrameViewer.h"

DualFrameViewer::DualFrameViewer() {
    m_graphicViewScale = 1.0;

    setWindowTitle("Dual Frame Viewer");

    auto rootLayout = new QVBoxLayout;

    auto topViewLayout = createSingleView(1, "Frame #1", m_topViewComboBox, m_topGraphicsView);
    auto bottomViewLayout = createSingleView(2, "Frame #2", m_bottomViewComboBox, m_bottomGraphicsView);

    m_topGraphicsView->setSynchronizedView(m_bottomGraphicsView);
    m_bottomGraphicsView->setSynchronizedView(m_topGraphicsView);

    rootLayout->addLayout(topViewLayout);
    rootLayout->addLayout(bottomViewLayout);

    this->setLayout(rootLayout);
    resize(1000, 800);
}

QVBoxLayout* DualFrameViewer::createSingleView(int id, QString title, QComboBox* &comboBox, SGraphicsView* &graphicsView) {
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
    auto frame = comboBox->currentData().value<std::shared_ptr<AbstractFrame>>();
    auto view = comboBox->property("ID").toInt() == 1 ? m_topGraphicsView : m_bottomGraphicsView;
    if (frame == nullptr) view->setScene(nullptr);
    else view->setScene(frame->getScene(2, 500));
}

void DualFrameViewer::setFrames(const std::vector<std::shared_ptr<AbstractFrame>> &frames) {
    m_topViewComboBox->clear();
    m_bottomViewComboBox->clear();
    m_topViewComboBox->addItem("Select", QVariant::fromValue<std::shared_ptr<AbstractFrame>>(nullptr));
    m_bottomViewComboBox->addItem("Select", QVariant::fromValue<std::shared_ptr<AbstractFrame>>(nullptr));
    for (const auto &frame: frames) {
        m_topViewComboBox->addItem(frame->getName(), QVariant::fromValue(frame));
        m_bottomViewComboBox->addItem(frame->getName(), QVariant::fromValue(frame));
    }
}
