#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <vector>
#include <string>

#include "ProgressReporter.h"
#include "RiverData.h"
#include "AbstractFrame.h"
#include "ReachNetwork.h"
#include "ReachNetworkDisplayFrame.h"


class Context : QObject {

    Q_OBJECT

private:
    std::shared_ptr<RiverData> m_riverData;
    std::vector<std::shared_ptr<ReachNetworkDisplayFrame>> m_frames;

private:
    std::shared_ptr<ProgressReporter> m_pr;

public:
    [[nodiscard]] const std::vector<std::shared_ptr<ReachNetworkDisplayFrame>>& getFrames() const { return m_frames; }

public:
    void openFrames(QStringList& fileNames);
    void openBoundary(const QString& fileName);
    void computeNetworkGraph();
    void buildAbstractionForAllFrames();

private:
    static std::shared_ptr<RiverFrame> loadFrame(const QString& file_name, Units& units);
    static std::shared_ptr<ReachNetwork> buildReachNetwork(const std::shared_ptr<NetworkGraph>& networkGraph);
};



#endif //CONTEXT_H
