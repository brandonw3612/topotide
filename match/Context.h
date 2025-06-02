#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <vector>

#include "MappingViewer.h"
#include "ProgressReporter.h"
#include "RiverData.h"
#include "ReachNetwork.h"
#include "NetworkDisplayFrame.h"
#include "ReachMapResult.h"


class Context : QObject {

    Q_OBJECT

private:
    std::shared_ptr<RiverData> m_riverData;
    std::vector<std::shared_ptr<NetworkDisplayFrame>> m_frames;

private:
    std::shared_ptr<ProgressReporter> m_pr;

public:
    [[nodiscard]] const std::vector<std::shared_ptr<NetworkDisplayFrame>>& getFrames() const { return m_frames; }

public:
    void openFrames(QStringList& fileNames);
    void openBoundary(const QString& fileName);
    void computeNetworkGraph();
    void buildAbstractionForAllFrames();
    void mapAllFrames(std::string outputFilePrefix, double sourceDeltaThreshold = 20.0, double targetDeltaThreshold = 2.0);
    MappingViewer* createMappingViewer(std::string resultPrefix, double sourceDeltaThreshold = 20.0, double targetDeltaThreshold = 2.0);

private:
    static std::shared_ptr<RiverFrame> loadFrame(const QString& file_name, Units& units);
    static std::map<int, ReachMapResult> mapNetworks(const std::shared_ptr<ReachNetwork>& network1,
                            const std::shared_ptr<ReachNetwork>& network2);
};



#endif //CONTEXT_H
