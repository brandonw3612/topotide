#ifndef MAPPINGVIEWER_H
#define MAPPINGVIEWER_H
#include <QComboBox>
#include <QWidget>

#include "MatchResultRenderer.h"
#include "PrecomputedDisplayFrame.h"

Q_DECLARE_METATYPE(std::shared_ptr<PrecomputedDisplayFrame>)

class MappingViewer : public QWidget {

    Q_OBJECT

private:
    QComboBox* m_comboBox;
    MatchResultRenderer* m_matchResultRenderer;
    std::vector<std::shared_ptr<PrecomputedDisplayFrame>> m_frames;

public:
    MappingViewer(const std::vector<std::shared_ptr<PrecomputedDisplayFrame>>& frames);

private:
    void onComboBoxChanged();
};



#endif //MAPPINGVIEWER_H
