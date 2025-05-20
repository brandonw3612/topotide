#ifndef REACH_H
#define REACH_H

#include <memory>
#include <vector>

#include "networkgraph.h"
#include "point.h"

/// A section of a river consisting of multiple segments with the same delta value.
class Reach {

public:
    /// A point in the reach associated with a unique index.
    struct IndexedPoint {
        /// Index of the point in the entire river framework.
        int index;
        /// Location of the point.
        Point location;

        /// Constructor for IndexedPoint.
        /// @param index The index of the point.
        /// @param location The location of the point.
        IndexedPoint(int index, const Point& location) : index(index), location(location) {}
    };

    /// Builder class for constructing a reach from edges in a network graph.
    class Builder {
    private:
        /// The reach being built.
        std::shared_ptr<Reach> m_reach;

    public:
        /// Constructs a Reach Builder from an edge.
        /// @param edge The edge to initialize the builder.
        explicit Builder(const NetworkGraph::Edge& edge);

        /// Appends an edge from a network graph to the reach.
        /// @param edge The edge to append.
        /// @return Whether the edge was successfully appended. If not, the current reach will not be affected.
        [[nodiscard]] bool appendEdge(const NetworkGraph::Edge& edge) const;

        /// Appends/Prepends another reach to the current reach, if possible.
        /// @param other Another reach builder to be appended/prepended in to the current one.
        /// @return Whether the other reach was successfully appended/prepended.
        /// If not, the current reach will not be affected.
        [[nodiscard]] bool pendReach(const std::shared_ptr<Builder>& other) const;

        /// Finalizes the reach and returns it.
        std::shared_ptr<Reach> build();

    private:
        /// Gets the front point of the reach.
        [[nodiscard]] IndexedPoint getFront() const { return m_reach->getFront(); }
        /// Gets the back point of the reach.
        [[nodiscard]] IndexedPoint getBack() const { return m_reach->getBack(); }
        /// Updates the bounds of the reach based on a new edge.
        void updateBounds(const NetworkGraph::Edge& newEdge) const;
        /// Updates the bounds of the reach based on another reach.
        void updateBounds(const std::shared_ptr<Builder> &other) const;

        template<typename T>
        static void prepend(std::vector<T>& v, const T& value) {
            v.insert(v.begin(), value);
        }
    };

private:
    /// Index of the reach in the network.
    int index = -1;
    /// Delta value of the reach.
    double m_delta = -std::numeric_limits<double>::infinity();
    /// Sequence of points in the reach.
    std::vector<Point> m_points;
    /// Sequence of intersection points along the reach.
    std::vector<IndexedPoint> m_intersectionPoints;
    /// Indices of intersection points in the original point sequence.
    std::vector<int> m_ipIndices;
    /// Lower bound of the reach.
    Point m_lowerBound;
    /// Upper bound of the reach.
    Point m_upperBound;

public:
    /// Gets the index of the reach.
    [[nodiscard]] int getIndex() const { return index; }
    /// Gets the delta value of the reach.
    [[nodiscard]] double getDelta() const { return m_delta; }
    /// Gets the sequence of points in the reach.
    [[nodiscard]] const std::vector<Point>& getPoints() const { return m_points; }
    /// Gets the sequence of intersection points along the reach.
    [[nodiscard]] const std::vector<IndexedPoint>& getIntersectionPoints() const { return m_intersectionPoints; }
    /// Gets the indices of intersection points in the original point sequence.
    [[nodiscard]] const std::vector<int>& getIpIndices() const { return m_ipIndices; }
    /// Gets the front point of the reach.
    [[nodiscard]] IndexedPoint getFront() const { return m_intersectionPoints.front(); }
    /// Gets the back point of the reach.
    [[nodiscard]] IndexedPoint getBack() const { return m_intersectionPoints.back(); }
    /// Gets the bounds of the reach.
    [[nodiscard]] std::pair<Point, Point> getBounds() const { return std::make_pair(m_lowerBound, m_upperBound); }
    [[nodiscard]] int findIndexedPoint(int index) const;
};

typedef std::shared_ptr<Reach> ReachPtr;
typedef const std::shared_ptr<Reach> ConstReachPtr;

#endif //REACH_H
