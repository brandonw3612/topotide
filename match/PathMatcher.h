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
    typedef std::vector<Point> Path;

public:
    class PathMatcherResult {
        public:
            Path path;
            std::vector<int> reachVertexIndices;
    };

private:
    struct EdgePathSegment {
        int m_edgeIndex;
        bool m_reversed;
    };

    typedef std::vector<double> DTWRow;

    Path m_inputPath;
    std::shared_ptr<NetworkGraph> m_graph;

    SetStack<int> m_vertexStack;
    VectorStack<EdgePathSegment> m_edgeStack;
    std::stack<DTWRow> m_dtwStack;

    double m_minDTWD;
    Path m_bestPath;
    std::vector<int> m_bestPathReachVertexIndices;

    std::unordered_set<int> m_ignoredEdges;
    std::priority_queue<std::pair<double, int>> m_lowerBoundEdgeDTWDistanceCostQueue;

private:
    PathMatcher(const Path& inputPath, const std::shared_ptr<NetworkGraph>& graph);
    PathMatcherResult computeClosestPath(double absoluteDistanceThreshold);
    [[nodiscard]] Path flattenPathStack() const;
    [[nodiscard]] std::vector<int> flattenPathStackReachVertexIndices() const;
    void dfs();
    void filterEdgesOnAbsoluteDistance(double absoluteDistanceThreshold);
    void initializeLowerBoundEdgeDTWDistanceCostQueue();


public:
    static std::vector<DTWRow> computeDTW(const Path& p1, const Path& p2);
    static PathMatcherResult match(const Path& inputPath, const std::shared_ptr<NetworkGraph>& graph, double absoluteDistanceThreshold);
    static Path matchVertexConnectedSegment(const Path& inputPath, const Path& inputReachSegment, const Path& matchedPath,
                                         const std::vector<int>& reachVertexIndices);
};



#endif //PATHMATCHER_H
