#ifndef REACHMAPRESULT_H
#define REACHMAPRESULT_H

#include <vector>
#include "point.h"

struct ReachMapResult {
    std::vector<Point> reach;
    std::vector<Point> reachPath;
    std::vector<Point> matchedPath;
    std::vector<int> matchedPathVertexIndices;
    std::pair<int, int> matchedSegmentRange;
    std::pair<int, int> fixedSegmentRange;
};

#endif //REACHMAPRESULT_H
