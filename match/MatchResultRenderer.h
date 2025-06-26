#ifndef MATCHRESULTRENDERER_H
#define MATCHRESULTRENDERER_H

#include <QWidget>

#include "PrecomputedDisplayFrame.h"
#include "SGraphicsView.h"

class MatchResultRenderer : public QWidget {

    Q_OBJECT

private:
    std::shared_ptr<PrecomputedDisplayFrame> m_frame;
    SGraphicsView *m_topGraphicsView, *m_middleGraphicsView, *m_bottomGraphicsView;

public:
    MatchResultRenderer();
    void setFrame(const std::shared_ptr<PrecomputedDisplayFrame>& frame);

private:
    void refreshViews();
};



#endif //MATCHRESULTRENDERER_H
