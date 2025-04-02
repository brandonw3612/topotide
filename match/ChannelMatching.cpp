#include "ChannelMatching.h"
#include <assert.h>
#include <climits>
#include <vector>
#include <algorithm>

static double pointDistance(const Point& p1, const Point& p2) {
    return std::sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

double ChannelMatching::DTWDistance(const std::shared_ptr<AbstractChannel>& channel1,
                                    const std::shared_ptr<AbstractChannel>& channel2) {
    const std::vector<Point>& points1 = channel1->getPoints();
    const std::vector<Point>& points2 = channel2->getPoints();
    int n = points1.size();
    int m = points2.size();

    std::vector<std::vector<double>> dtw(n, std::vector<double>(m, std::numeric_limits<double>::infinity()));
    dtw[0][0] = 0;
    for (int i = 1; i < n; i++) {
        for (int j = 1; j < m; j++) {
            double cost = pointDistance(points1[i - 1], points2[j - 1]);
            dtw[i][j] = cost + std::min({dtw[i - 1][j], dtw[i][j - 1], dtw[i - 1][j - 1]});
        }
    }
    return dtw[n - 1][m - 1];
}

std::vector<std::pair<Point, double>> ChannelMatching::DTWMatchList(const std::shared_ptr<AbstractChannel>& channel1,
                                    const std::shared_ptr<AbstractChannel>& channel2) {
    const std::vector<Point>& points1 = channel1->getPoints();
    const std::vector<Point>& points2 = channel2->getPoints();
    int n = points1.size();
    int m = points2.size();

    std::vector<std::vector<double>> dtw(n, std::vector<double>(m, std::numeric_limits<double>::infinity()));
    std::vector<std::vector<std::pair<int, int>>> lastPoint(n, std::vector<std::pair<int, int>>(m));

    dtw[0][0] = 0;
    for (int i = 1; i < n; i++) {
        for (int j = 1; j < m; j++) {
            double cost = pointDistance(points1[i - 1], points2[j - 1]);
            double minLast = dtw[i - 1][j];
            std::pair<int, int> last = {i - 1, j};

            if (dtw[i][j - 1] < minLast) {
                minLast = dtw[i][j - 1];
                last = {i, j - 1};
            }

            if (dtw[i - 1][j - 1] < minLast) {
                minLast = dtw[i - 1][j - 1];
                last = {i - 1, j - 1};
            }

            dtw[i][j] = cost + minLast;
            lastPoint[i][j] = last;
        }
    }
    std::vector<std::pair<Point, double>> matchedTo;
    int i = n - 1, j = m - 1;
    while (i > 0 || j > 0) {
        double cost = pointDistance(points1[i], points2[j]);
        matchedTo.push_back({points2[lastPoint[i][j].second], cost});
        std::tie(i, j) = lastPoint[i][j];
    }
//    while (i > 0){
//        double cost = pointDistance(points1[i], points2[0]);
//        matchedTo.push_back({points2[lastPoint[i][0].second], cost});
//        i = i-1;
//    }
//    while (j > 0){
//      double cost = pointDistance(points1[0], points2[j]);
//      matchedTo.push_back({points2[lastPoint[i][j].second], cost});
//      j = j-1;
//    }

    std::reverse(matchedTo.begin(), matchedTo.end());
    return matchedTo;
}

std::shared_ptr<AbstractChannel> ChannelMatching::FindBestMatch(
    const std::shared_ptr<AbstractChannel>& query,const std::shared_ptr<AbstractGraph>& graph,
    int maxDepth, double minDelta) {
    double bestDist = std::numeric_limits<double>::infinity();
    std::shared_ptr<AbstractChannel> bestMatch = nullptr;

    // TODO: Adjust to limit the number of channels using delta and only get distinct channels, not per node
    std::map<std::shared_ptr<AbstractGraph::AGNode>, int> channelDepthMap;
    std::map<std::shared_ptr<AbstractGraph::AGNode>, int> nodeDepthMap;
    for (const auto& node : graph->getNodes()) {

        const auto& next = node->getChannel();
        double dist = DTWDistance(query, next);
        if (dist < bestDist) {
            bestDist = dist;
            bestMatch = next;
        }
    }

    return bestMatch;
}
