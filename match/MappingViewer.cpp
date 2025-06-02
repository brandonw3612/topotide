//
// Created by Brandon Wong on 6/3/25.
//

#include "MappingViewer.h"

#include <QVBoxLayout>

MappingViewer::MappingViewer(const std::vector<std::shared_ptr<PrecomputedDisplayFrame>> &frames) {
    setWindowTitle("Mapping Viewer");

    QVBoxLayout* layout = new QVBoxLayout;
    m_comboBox = new QComboBox;
    m_comboBox->setMinimumWidth(400);
    connect(m_comboBox, &QComboBox::currentIndexChanged, this, &MappingViewer::onComboBoxChanged);
    layout->addWidget(m_comboBox);

    m_matchResultRenderer = new MatchResultRenderer;
    layout->addWidget(m_matchResultRenderer, 1);

    this->setLayout(layout);

    for (auto frame : frames) {
        m_comboBox->addItem(
            frame->getName(),
            QVariant::fromValue(frame)
        );
    }
}

void MappingViewer::onComboBoxChanged() {
    auto frame = m_comboBox->currentData().value<std::shared_ptr<PrecomputedDisplayFrame>>();
    m_matchResultRenderer->setFrame(frame);
}
