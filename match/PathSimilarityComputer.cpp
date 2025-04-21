#include "PathSimilarityComputer.h"
#include <cassert>
#include <algorithm>


std::vector<Point> PathSimilarityComputer::computeMostSimilarPath(std::vector<Point> inputPath)
{
    StatusStructure statusStructure;
    auto& startingVertices = m_graph.m_networkGraph;
    for (int i = 0; i < m_graph.m_networkGraph.vertexCount(); i++) {
        auto& startingVertex = m_graph.m_networkGraph[i];
        std::vector<double> DTW_init(inputPath.size());
        DTW_init[0] = startingVertex.p.distanceTo(inputPath[0]);
        for (int i = 1; i < DTW_init.size(); i++) {
            DTW_init[i] += DTW_init[i - 1] + startingVertex.p.distanceTo(inputPath[i]);
        }
        std::unordered_set<int> verticesInsidePath {startingVertex.id};
        std::vector<int> verticesPath {startingVertex.id};

        for (auto& edgeID : m_graph.adjacencyList[startingVertex.id]) {
            auto& edge = m_graph.m_networkGraph.edge(edgeID);
            if (verticesInsidePath.count(edge.to)) continue;
            recursiveComputeMostSimilarPath(edge, verticesInsidePath, verticesPath, DTW_init, statusStructure, inputPath);
        }
    }

    std::vector<Point> mostSimilarPath;
    for (int i = 0; i < statusStructure.verticesPath.size() - 1; i++) {
        int startVertexID = statusStructure.verticesPath[i];
        int endVertexID = statusStructure.verticesPath[i + 1];
        bool edgeFound = false;
        for (auto& edgeID : m_graph.adjacencyList[startVertexID]) {
            auto& edge = m_graph.m_networkGraph.edge(edgeID);
            if (edge.to != endVertexID) continue;

            edgeFound = true;
            if (mostSimilarPath.size()) mostSimilarPath.pop_back();
            mostSimilarPath.insert(mostSimilarPath.end(), edge.path.begin(), edge.path.end());
            break;
        }
        assert(edgeFound);
    }

    return mostSimilarPath;
}

void PathSimilarityComputer::recursiveComputeMostSimilarPath(NetworkGraph::Edge& edge, std::unordered_set<int> verticesInsidePath, 
                                                            std::vector<int> verticesPath, std::vector<double> DTW_init,
                                                             StatusStructure &statusStructure, std::vector<Point>& inputPath) {
    verticesInsidePath.insert(edge.to);
    verticesPath.push_back(edge.to);
    std::vector<std::vector<double>> DTW(edge.path.size(), std::vector<double>(inputPath.size()));
    DTW[0] = DTW_init;
    for (int i = 1; i < DTW.size(); i++) {
        DTW[i][0] = DTW[i - 1][0] + edge.path[i].distanceTo(inputPath[0]);
    }
    for (int i = 1 ; i < DTW.size(); i++) {
        for (int j = 1; j < inputPath.size(); j++) {
            double prevMin = std::min({DTW[i - 1][j], DTW[i][j - 1], DTW[i - 1][j - 1]});
            DTW[i][j] = prevMin + edge.path[i].distanceTo(inputPath[j]);
        }
    }
    double finalDTWValue = DTW.back().back();
    if (statusStructure.lowestDTW > finalDTWValue) {
        statusStructure.lowestDTW = finalDTWValue;
        statusStructure.verticesPath = verticesPath;
    }

    for (auto& checkEdgeID : m_graph.adjacencyList[edge.to]) {
        auto& checkEdge = m_graph.m_networkGraph.edge(checkEdgeID);
        if (verticesInsidePath.count(checkEdge.to)) continue;
        recursiveComputeMostSimilarPath(checkEdge, verticesInsidePath, verticesPath, DTW.back(), statusStructure, inputPath);
    }
}
