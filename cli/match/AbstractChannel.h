#ifndef TOPOTIDE_ABSTRACTCHANNEL_H
#define TOPOTIDE_ABSTRACTCHANNEL_H

#include <vector>
#include "point.h"
#include "networkgraph.h"

class AbstractChannel {
private:
    std::vector<Point> m_points;
    std::vector<int> m_pointIDs;
    int m_from, m_to;

public:
    explicit AbstractChannel(const NetworkGraph::Edge& edge);

public:
    /**
     * Appends an edge from a network graph to the channel.
     * @param edge The edge to append.
     *
     * @return Whether the edge was successfully appended.
     */
    bool appendEdge(const NetworkGraph::Edge& edge);

    /**
     * Merges another channel to the current channel, if possible.
     * @param other Another channel to be merged in to the current one.
     * @return Whether the merging succeeded.
     */
    bool mergeChannel(const std::shared_ptr<AbstractChannel>& other);

    [[nodiscard]] int getLength() const;
    [[nodiscard]] const std::vector<int>& getPointIDs() const;
    [[nodiscard]] int getStartPointID() const;
    [[nodiscard]] int getEndPointID() const;
};


#endif //TOPOTIDE_ABSTRACTCHANNEL_H
