#include "SubdividedAbstractGraph.h"
#include <unordered_set>
#include <set>
#include <algorithm>

SubdividedAbstractGraph::SubdividedAbstractGraph(std::shared_ptr<AbstractGraph> abstractGraph, int maxDepth, double minDelta) {
    std::unordered_set<std::shared_ptr<AbstractGraph::AGNode>> channelsProcessed;
    std::unordered_map<int, std::vector<std::shared_ptr<AbstractGraph::AGNode>>> depthToChannels;
    channelsProcessed.insert(nullptr);
    for (int i = 0; i <= maxDepth; i++) {
        for (auto& node : abstractGraph->getNodes()) {
            if (channelsProcessed.count(node)) continue;
            if (!channelsProcessed.count(node->getMergingParent()) || !channelsProcessed.count(node->getSplittingParent())) continue;
            if (node->getChannel()->getDelta() < minDelta) continue;
            channelsProcessed.insert(node);
            depthToChannels[i].push_back(node);
        }
    }

    std::unordered_map<std::shared_ptr<AbstractGraph::AGNode>, std::set<Point>> channelToSplittingPoints;
    for (int i = maxDepth; i >= 0; i--) {
        for (auto& node : depthToChannels[i]) {
            channelToSplittingPoints[node->getSplittingParent()].insert(node->getChannel()->getStartPoint());
            channelToSplittingPoints[node->getMergingParent()].insert(node->getChannel()->getEndPoint());
        }
    }
    if (channelToSplittingPoints.count(nullptr)) channelToSplittingPoints.erase(nullptr);

    std::set<Point> allPoints;
    for (int i = maxDepth; i >= 0; i--) {
        for (auto& node : depthToChannels[i]) {
            allPoints.insert(node->getChannel()->getStartPoint());
            allPoints.insert(node->getChannel()->getEndPoint());
        }
    }
    std::map<Point, int> pointToID;
    for (auto& p : allPoints) {
        pointToID[p] = m_networkGraph.addVertex(p);
    }

    for (int i = maxDepth; i >= 0; i--) {
        for (auto& node : depthToChannels[i]) {
            auto delta = node->getChannel()->getDelta();
            auto points = node->getChannel()->getPoints();
            auto splittingPoints = channelToSplittingPoints[node];
            if (splittingPoints.count(points[0])) splittingPoints.erase(points[0]);
            if (splittingPoints.count(points.back())) splittingPoints.erase(points.back());

            int edgeID;
            auto prevPoint = points[0];
            int prevPointIndex = 0;
            for (int j = 1; j < points.size(); j++) {
                auto& p = points[j];
                if (!splittingPoints.count(p)) continue;
                
                insertEdge(points, pointToID, prevPointIndex, j, i, delta);
                insertEdge(points, pointToID, j, prevPointIndex, i, delta);

                prevPoint = p;
                prevPointIndex = j;
                splittingPoints.erase(p);
            }

            insertEdge(points, pointToID, prevPointIndex, points.size() - 1, i, delta);
            insertEdge(points, pointToID, points.size() - 1, prevPointIndex, i, delta);
        }
    }
}

void SubdividedAbstractGraph::insertEdge(std::vector<Point> &points, std::map<Point, int> pointToID, int startIndex, int endIndex, int depth, double delta) {
    std::vector<Point> edgePoints(points.begin() + std::min(startIndex, endIndex), points.begin() + std::max(startIndex, endIndex) + 1);
    if (startIndex > endIndex) reverse(edgePoints.begin(), edgePoints.end());
    auto startPointID = pointToID[points[startIndex]];
    auto endPointID = pointToID[points[endIndex]];
    int edgeID = m_networkGraph.addEdge(startPointID, endPointID, 
                edgePoints,
                delta);
    adjacencyList[startPointID].push_back(edgeID);
    edgeDepth[edgeID] = depth;
}
