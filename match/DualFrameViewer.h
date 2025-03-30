#ifndef TOPOTIDE_DUALFRAMEVIEWER_H
#define TOPOTIDE_DUALFRAMEVIEWER_H

#include <QWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include <QGraphicsView>
#include "AbstractFrame.h"
#include "SGraphicsView.h"

Q_DECLARE_METATYPE(std::shared_ptr<AbstractFrame>)

class DualFrameViewer : public QWidget {

    Q_OBJECT

private:
    double m_graphicViewScale;

private:
    QComboBox *m_topViewComboBox, *m_bottomViewComboBox;
    SGraphicsView *m_topGraphicsView, *m_bottomGraphicsView;

public:
    DualFrameViewer();

public:
    void setFrames(const std::vector<std::shared_ptr<AbstractFrame>> &frames);

private:
    void onComboBoxChanged();
    void onGraphicViewWheelChanged();

private:
    QVBoxLayout* createSingleView(int id, QString title, QComboBox* &comboBox, SGraphicsView* &graphicsView);
};


#endif //TOPOTIDE_DUALFRAMEVIEWER_H
