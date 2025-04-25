#include "PathMatcher.h"

PathMatcher::PathMatcher(const Path &inputPath, const std::shared_ptr<NetworkGraph> &graph) {
    m_inputPath = inputPath;
    m_graph = graph;
    m_minDTWD = std::numeric_limits<double>::infinity();
}

const PathMatcher::Path &PathMatcher::computeClosestPath() {
    m_minDTWD = std::numeric_limits<double>::infinity();
    m_bestPath.clear();

    for (int i = 0; i < m_graph->vertexCount(); ++i) {
        m_vertexStack.clear();
        m_edgeStack.clear();
        m_dtwStack = std::stack<DTWRow>();

        auto vertex = (*m_graph)[i];
        m_vertexStack.push(vertex.id);
        DTWRow firstRow(m_inputPath.size());
        firstRow[0] = m_inputPath[0].distanceTo(vertex.p);
        for (int j = 1; j < firstRow.size(); ++j) {
            firstRow[j] = firstRow[j - 1] + m_inputPath[j].distanceTo(vertex.p);
        }
        m_dtwStack.push(firstRow);

        dfs();
    }

    return m_bestPath;
}

void PathMatcher::dfs() {
    const auto currentVertexIndex = m_vertexStack.top();
    for (const auto& edgeID : (*m_graph)[currentVertexIndex].incidentEdges) {
        const auto& edge = m_graph->edge(edgeID);
        auto otherEnd = edge.from + edge.to - currentVertexIndex;
        if (m_vertexStack.contains(otherEnd)) continue;

        m_vertexStack.push(otherEnd);
        m_edgeStack.push({edgeID, edge.from == otherEnd});
        DTWRow lastRow(m_dtwStack.top()), currentRow(m_inputPath.size());
        if (currentVertexIndex == edge.from) {
            for (int i = 1; i < edge.path.size(); ++i) {
                currentRow[0] = lastRow[0] + m_inputPath[0].distanceTo(edge.path[i]);
                for (int j = 1; j < m_inputPath.size(); ++j) {
                    const double prevMin = std::min({lastRow[j], currentRow[j - 1], lastRow[j - 1]});
                    currentRow[j] = prevMin + m_inputPath[j].distanceTo(edge.path[i]);
                }
                lastRow = currentRow;
            }
        } else {
            for (int i = edge.path.size() - 2; i >= 0; --i) {
                currentRow[0] = lastRow[0] + m_inputPath[0].distanceTo(edge.path[i]);
                for (int j = 1; j < m_inputPath.size(); ++j) {
                    const double prevMin = std::min({lastRow[j], currentRow[j - 1], lastRow[j - 1]});
                    currentRow[j] = prevMin + m_inputPath[j].distanceTo(edge.path[i]);
                }
                lastRow = currentRow;
            }
        }
        const double finalDTW = currentRow.back();
        if (finalDTW < m_minDTWD) {
            m_minDTWD = finalDTW;
            m_bestPath = flattenPathStack();
        }
        m_dtwStack.push(currentRow);

        dfs();

        m_dtwStack.pop();
        m_edgeStack.pop();
        m_vertexStack.pop();
    }
}

PathMatcher::Path PathMatcher::flattenPathStack() const {
    Path result;
    for (const auto&[m_edgeIndex, m_reversed] : m_edgeStack.asVector()) {
        const auto& edgePath = m_graph->edge(m_edgeIndex).path;
        if (m_reversed) {
            result.insert(result.end(), edgePath.rbegin(), edgePath.rend());
        } else {
            result.insert(result.end(), edgePath.begin(), edgePath.end());
        }
    }
    return result;
}

const PathMatcher::Path & PathMatcher::match(const Path &inputPath, const std::shared_ptr<NetworkGraph> &graph) {
    static PathMatcher matcher(inputPath, graph);
    return matcher.computeClosestPath();
}