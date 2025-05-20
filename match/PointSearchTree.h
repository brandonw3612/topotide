#ifndef POINTSEARCHTREE_H
#define POINTSEARCHTREE_H

#include <vector>

#include "point.h"
#include "ReachNetwork.h"

#define SEARCH_GRID_LENGTH 8

class PointSearchTree {
private:
    struct PointData {
        double x, y;
        int reachId;
    };

    class Node {
    public:
        bool m_isLeaf;
        int m_dimension;
        double m_splitValue;
        std::vector<PointData> m_points;
        std::shared_ptr<Node> m_upper;
        std::shared_ptr<Node> m_lower;

    private:
        explicit Node(const std::vector<PointData> &points) : m_isLeaf(true), m_points(points) { }

        explicit Node(int dimension, double splitValue, std::shared_ptr<Node> upper, std::shared_ptr<Node> lower) :
            m_isLeaf(false), m_dimension(dimension), m_splitValue(splitValue), m_upper(upper), m_lower(lower) { }

    public:
        static std::shared_ptr<Node> create(std::vector<PointData>& points, double gridLength, int dimension = 0);
    };

private:
    std::shared_ptr<Node> m_root;

public:
    explicit PointSearchTree(const std::shared_ptr<ReachNetwork>& network, int maxDepth, double minDelta);
    int findReach(double x, double y) const;
};


#endif //POINTSEARCHTREE_H
