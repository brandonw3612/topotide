#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "AbstractChannel.h"
#include "AbstractGraph.h"
#include "point.h"

struct ChannelMatching {
    static double DTWDistance(const std::shared_ptr<AbstractChannel>& channel1,
                              const std::shared_ptr<AbstractChannel>& channel2);

    static std::vector<std::pair<Point, double>> DTWMatchList(const std::shared_ptr<AbstractChannel>& channel1,
                              const std::shared_ptr<AbstractChannel>& channel2);

    static std::shared_ptr<AbstractChannel> FindBestMatch(
        const std::shared_ptr<AbstractChannel>& query,
        const std::shared_ptr<AbstractGraph>& graph,
        int maxDepth, double minDelta);
};
