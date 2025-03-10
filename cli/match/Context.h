#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <vector>
#include <string>

#include "ProgressReporter.h"
#include "RiverData.h"


class Context : QObject {

    Q_OBJECT

private:
    std::shared_ptr<RiverData> m_riverData;

private:
    std::shared_ptr<ProgressReporter> m_pr;

public:
    void openFrames(QStringList& fileNames);
    void openBoundary(const QString& fileName);
    void compute();

private:
    static std::shared_ptr<RiverFrame> loadFrame(const QString& file_name, Units& units);
};



#endif //CONTEXT_H
