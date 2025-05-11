#include <algorithm>
#include <numeric>

#include "PathMatcher.h"

PathMatcher::PathMatcher(const Path &inputPath, const std::shared_ptr<NetworkGraph> &graph) {
    m_inputPath = inputPath;
    m_graph = graph;
    m_minDTWD = std::numeric_limits<double>::infinity();
}

const PathMatcher::Path &PathMatcher::computeClosestPath(double absoluteDistanceThreshold) {
    m_minDTWD = std::numeric_limits<double>::infinity();
    m_bestPath.clear();
    m_ignoredEdges.clear();

    filterEdgesOnAbsoluteDistance(absoluteDistanceThreshold);
    initializeLowerBoundEdgeDTWDistanceCostQueue();

    std::vector<int> vertexOrder(m_graph->vertexCount());
    std::iota(vertexOrder.begin(), vertexOrder.end(), 0);
    std::sort(vertexOrder.begin(), vertexOrder.end(), [this](const auto& a, const auto& b) {
        auto vertexA = (*m_graph)[a];
        auto vertexB = (*m_graph)[b];
        auto startingPoint = m_inputPath[0];
        return vertexA.p.distanceTo(startingPoint) < vertexB.p.distanceTo(startingPoint);
    });

    for (int i = 0; i < m_graph->vertexCount(); ++i) {
        m_vertexStack.clear();
        m_edgeStack.clear();
        m_dtwStack = std::stack<DTWRow>();

        auto vertex = (*m_graph)[vertexOrder[i]];
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
        if (m_ignoredEdges.count(edgeID)) continue;

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
            // Filter out edges that can never be included
            while (m_lowerBoundEdgeDTWDistanceCostQueue.size() && m_lowerBoundEdgeDTWDistanceCostQueue.top().first > m_minDTWD) {
                auto& p = m_lowerBoundEdgeDTWDistanceCostQueue.top();
                m_lowerBoundEdgeDTWDistanceCostQueue.pop();
                m_ignoredEdges.insert(p.second);
            }
        }
        m_dtwStack.push(currentRow);

        bool canImprove = false;
        for (auto& d : currentRow) {
            if (d <= m_minDTWD) {
                canImprove = true;
                break;
            }
        }

        if (canImprove) {
            dfs();
        }

        m_dtwStack.pop();
        m_edgeStack.pop();
        m_vertexStack.pop();
    }
}

void PathMatcher::filterEdgesOnAbsoluteDistance(double absoluteDistanceThreshold) {
    int count = 0;
    for (int i = 0; i < (*m_graph).edgeCount(); i++) {
        auto& edge = (*m_graph).edge(i);
        double edgeDistance = std::numeric_limits<double>::infinity();
        for (auto& p1 : edge.path) {
            for (auto& p2 : m_inputPath) {
                edgeDistance = std::min(edgeDistance, p1.distanceTo(p2));
            }
        }
        if (edgeDistance <= absoluteDistanceThreshold) continue;
        count++;
        m_ignoredEdges.insert(i);
    }
    std::cout << "Edges filtered based on absolute distance: " << count << std::endl;
}

void PathMatcher::initializeLowerBoundEdgeDTWDistanceCostQueue() {
    m_lowerBoundEdgeDTWDistanceCostQueue = std::priority_queue<std::pair<double, int>>();
    for (int i = 0; i < (*m_graph).edgeCount(); i++) {
        auto& edge = (*m_graph).edge(i);
        double lowerBound = 0;
        for (auto& p1 : edge.path) {
            double bestDistance = std::numeric_limits<double>::infinity();
            for (auto& p2 : m_inputPath) {
                bestDistance = std::min(bestDistance, p1.distanceTo(p2));
            }
            lowerBound += bestDistance;
        }
        m_lowerBoundEdgeDTWDistanceCostQueue.push({lowerBound, i});
        // Most edges with a high lower bound are already filtered by the absolute distance filter
        // This optimization might not actually lead to any improvement since edges are a lot smaller for lower deltas,
        // which leads to lower lower bounds
        // std::cout << lowerBound << " " << m_ignoredEdges.count(i) << std::endl;
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

const PathMatcher::Path & PathMatcher::match(const Path &inputPath, const std::shared_ptr<NetworkGraph> &graph, double absoluteDistanceThreshold) {
    static PathMatcher matcher(inputPath, graph);
    return matcher.computeClosestPath(absoluteDistanceThreshold);
}