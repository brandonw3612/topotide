#ifndef PATHMATCHER_H
#define PATHMATCHER_H

#include <memory>
#include <stack>
#include <queue>
#include <unordered_set>

#include "networkgraph.h"
#include "utils/SetStack.hpp"
#include "utils/VectorStack.hpp"

class PathMatcher {
private:
    struct EdgePathSegment {
        int m_edgeIndex;
        bool m_reversed;
    };

    typedef std::vector<double> DTWRow;
    typedef std::vector<Point> Path;

    Path m_inputPath;
    std::shared_ptr<NetworkGraph> m_graph;

    SetStack<int> m_vertexStack;
    VectorStack<EdgePathSegment> m_edgeStack;
    std::stack<DTWRow> m_dtwStack;

    double m_minDTWD;
    Path m_bestPath;

    std::unordered_set<int> m_ignoredEdges;
    std::priority_queue<std::pair<double, int>> m_lowerBoundEdgeDTWDistanceCostQueue;

private:
    PathMatcher(const Path& inputPath, const std::shared_ptr<NetworkGraph>& graph);
    Path computeClosestPath(double absoluteDistanceThreshold);
    [[nodiscard]] Path flattenPathStack() const;
    void dfs();
    void filterEdgesOnAbsoluteDistance(double absoluteDistanceThreshold);
    void initializeLowerBoundEdgeDTWDistanceCostQueue();

public:
    static Path match(const Path& inputPath, const std::shared_ptr<NetworkGraph>& graph, double absoluteDistanceThreshold);
    static Path matchSegment(const Path& inputPath, const Path& inputReachSegment, const Path& matchedPath);
};



#endif //PATHMATCHER_H
