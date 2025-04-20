#ifndef TOPOTIDE_ABSTRACTCHANNEL_H
#define TOPOTIDE_ABSTRACTCHANNEL_H

#include <memory>
#include <vector>
#include "point.h"
#include "networkgraph.h"

/// Abstract channel in an abstract graph.
class AbstractChannel {
private:
    std::vector<Point> m_points;
    std::vector<int> m_pointIDs;
    int m_from, m_to;
    Point m_lowerBound, m_upperBound;
    double m_delta;

public:
    /// Constructs an AbstractChannel object from a segment.
    /// @param edge The edge to initialize the channel.
    explicit AbstractChannel(const NetworkGraph::Edge& edge);

public:
    /// Gets the delta value of the channel.
    [[nodiscard]] double getDelta() const { return m_delta; }

    /// Gets the number of points in the channel.
    [[nodiscard]] size_t getLength() const { return m_points.size(); }

    /// Gets the list of points along the channel.
    [[nodiscard]] const std::vector<Point>& getPoints() const { return m_points; }

    /// Gets the ID list of points along the channel.
    [[nodiscard]] const std::vector<int>& getPointIDs() const { return m_pointIDs; }

    /// Gets the ID of the starting point of the channel.
    [[nodiscard]] int getStartPointID() const { return m_from; }

    /// Gets the ID of the ending point of the channel.
    [[nodiscard]] int getEndPointID() const { return m_to; }

    /// Gets the rectangular bound of the channel.
    /// @return \b result.first -- the lower bound; \n
    /// \b result.second -- the upper bound.
    [[nodiscard]] std::pair<Point, Point> getBounds() const {
        return std::make_pair(m_lowerBound, m_upperBound);
    }

public:
    /// Appends an edge from a network graph to the channel.
    /// @param edge The edge to append.
    /// @return Whether the edge was successfully appended. If not, the current channel will not be affected.
    bool appendEdge(const NetworkGraph::Edge& edge);

    /// Merges another channel to the current channel, if possible.
    /// @param other Another channel to be merged in to the current one.
    /// @return Whether the other channel was successfully merged. If not, the current channel will not be affected.
    bool mergeChannel(const std::shared_ptr<AbstractChannel>& other);

private:
    void updateBounds(const NetworkGraph::Edge& newEdge);
    void updateBounds(const std::shared_ptr<AbstractChannel> &other);
};


#endif //TOPOTIDE_ABSTRACTCHANNEL_H
