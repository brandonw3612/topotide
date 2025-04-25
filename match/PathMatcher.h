#ifndef PATHMATCHER_H
#define PATHMATCHER_H

#include <memory>
#include <stack>

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

private:
    PathMatcher(const Path& inputPath, const std::shared_ptr<NetworkGraph>& graph);
    const Path& computeClosestPath();
    [[nodiscard]] Path flattenPathStack() const;
    void dfs();

public:
    static const Path& match(const Path& inputPath, const std::shared_ptr<NetworkGraph>& graph);
};



#endif //PATHMATCHER_H
