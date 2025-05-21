#include <ranges>
#include <algorithm>

#include "PointSearchTree.h"

std::shared_ptr<PointSearchTree::Node> PointSearchTree::Node::create(std::vector<PointData> &points, double gridLength,
        int dimension) {
    auto minX = std::numeric_limits<double>::max(), maxX = std::numeric_limits<double>::lowest();
    auto minY = std::numeric_limits<double>::max(), maxY = std::numeric_limits<double>::lowest();
    for (const auto& point: points) {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }
    if (points.size() <= 1 || maxX - minX <= 2 * gridLength && maxY - minY <= 2 * gridLength) {
        return std::shared_ptr<Node>(new Node(points));
    }
    if (maxX - minX <= 2 * gridLength) dimension = 1;
    if (maxY - minY <= 2 * gridLength) dimension = 0;
    if (dimension == 0) {
        auto split = 1.0 / 2.0 * (minX + maxX);
        std::vector<PointData> upper, lower;
        std::ranges::copy_if(points, std::back_inserter(upper), [split, gridLength](const PointData& p) { return p.x <= split + gridLength; });
        std::ranges::copy_if(points, std::back_inserter(lower), [split, gridLength](const PointData& p) { return p.x >= split - gridLength; });
        auto upperNode = create(upper, gridLength, 1);
        auto lowerNode = create(lower, gridLength, 1);
        return std::shared_ptr<Node>(new Node(0, split, upperNode, lowerNode));
    }
    auto split = 1.0 / 2.0 * (minY + maxY);
    std::vector<PointData> upper, lower;
    std::ranges::copy_if(points, std::back_inserter(upper), [split, gridLength](const PointData& p) { return p.y <= split + gridLength; });
    std::ranges::copy_if(points, std::back_inserter(lower), [split, gridLength](const PointData& p) { return p.y >= split - gridLength; });
    auto upperNode = create(upper, gridLength, 0);
    auto lowerNode = create(lower, gridLength, 0);
    return std::shared_ptr<Node>(new Node(1, split, upperNode, lowerNode));
}

PointSearchTree::PointSearchTree(const std::shared_ptr<ReachNetwork> &network, int maxDepth, double minDelta) {
    std::vector<PointData> points;
    for (const auto& [_, node] : network->getNodes()) {
        if (node->getDepth() > maxDepth || node->getReach()->getDelta() < minDelta) continue;
        for (auto point : node->getReach()->getPoints()) {
            points.emplace_back(point.x, point.y, node->getReach()->getIndex());
        }
    }
    m_root = Node::create(points, SEARCH_GRID_LENGTH);
}

int PointSearchTree::findReach(double x, double y) const {
    auto current = m_root;
    while (!current->m_isLeaf) {
        if (current->m_dimension == 0) {
            if (x < current->m_splitValue) {
                current = current->m_upper;
            } else {
                current = current->m_lower;
            }
        } else {
            if (y < current->m_splitValue) {
                current = current->m_upper;
            } else {
                current = current->m_lower;
            }
        }
    }
    int reachId = -1;
    double dist = std::numeric_limits<double>::max();
    for (const auto& point : current->m_points) {
        auto d = fabs(point.x - x) + fabs(point.y - y);
        if (d < dist) {
            dist = d;
            reachId = point.reachId;
        }
    }
    return dist < SEARCH_GRID_LENGTH ? reachId : -1;
}
