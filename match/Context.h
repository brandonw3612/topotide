#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <vector>
#include <string>

#include "AbstractChannel.h"
#include "ProgressReporter.h"
#include "RiverData.h"
#include "AbstractGraph.h"
#include "AbstractFrame.h"


class Context : QObject {

    Q_OBJECT

private:
    std::shared_ptr<RiverData> m_riverData;
    std::vector<std::shared_ptr<AbstractFrame>> m_frames;

private:
    std::shared_ptr<ProgressReporter> m_pr;

public:
    const std::vector<std::shared_ptr<AbstractFrame>>& getFrames() const { return m_frames; }

public:
    void openFrames(QStringList& fileNames);
    void openBoundary(const QString& fileName);
    void computeNetworkGraph();
    void buildAbstractionForAllFrames();

private:
    static std::shared_ptr<RiverFrame> loadFrame(const QString& file_name, Units& units);
    static std::shared_ptr<AbstractGraph> buildAbstraction(const std::shared_ptr<NetworkGraph>& networkGraph);
    static void mergeChannels(std::vector<std::shared_ptr<AbstractChannel>>& channels,
                              const std::vector<std::shared_ptr<AbstractChannel>>::iterator& channel);
    static std::vector<std::shared_ptr<AbstractChannel>> buildAbstractChannelFromEdges(const std::vector<NetworkGraph::Edge>& edges);
};



#endif //CONTEXT_H
