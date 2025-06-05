#ifndef REACHMAPRESULT_H
#define REACHMAPRESULT_H

#include <vector>
#include "point.h"

struct ReachMapResult {
    std::vector<Point> reach;
    std::vector<Point> reachPath;
    std::vector<Point> matchedPath;
    std::vector<Point> vertexConnectedMatchedSegment;
};

#endif //REACHMAPRESULT_H
