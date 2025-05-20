#ifndef TOPOTIDE_DUALFRAMEVIEWER_H
#define TOPOTIDE_DUALFRAMEVIEWER_H

#include <QWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QGraphicsView>

#include "Frame.h"
#include "NetworkDisplayFrame.h"
#include "SGraphicsView.h"

// Q_DECLARE_METATYPE(std::shared_ptr<AbstractFrame>)
Q_DECLARE_METATYPE(std::shared_ptr<NetworkDisplayFrame>)

class DualFrameViewer : public QWidget {

    Q_OBJECT

private:
    QComboBox *m_topViewComboBox, *m_bottomViewComboBox;
    SGraphicsView *m_topGraphicsView, *m_bottomGraphicsView;
    std::shared_ptr<NetworkDisplayFrame> m_topFrame, m_bottomFrame;
    QSpinBox *m_depthSpinBox;
    QLineEdit *m_deltaLineEdit;

public:
    DualFrameViewer();

public:
    void setFrames(const std::vector<std::shared_ptr<NetworkDisplayFrame>> &frames);
    void refreshViews();

private:
    void onComboBoxChanged();
    void onFilterChanged();
    void onMatchReachPath();

private:
    QVBoxLayout* createSingleView(int id, const QString& title, QComboBox* &comboBox, SGraphicsView* &graphicsView);
};


#endif //TOPOTIDE_DUALFRAMEVIEWER_H
