#ifndef PRECOMPUTEDREACHNETWORK_H
#define PRECOMPUTEDREACHNETWORK_H

#include "ReachNetwork.h"
#include <fstream>
#include <ranges>

class PreComputedReachNetwork final : public ReachNetwork {
private:
    struct Precomputed {
        int nodeIndex;
        std::vector<Point> reachPath;
        std::vector<Point> matchedSegment;
        std::vector<Point> fixedSegment;
        std::vector<Point> matchedPath;
    };
    std::map<int, Precomputed> m_precomputed;

    friend class ReachNetwork;

private:
    PreComputedReachNetwork() = default;

public:
    std::shared_ptr<ReachNetwork> asReachNetwork();

    std::ranges::elements_view<std::ranges::ref_view<std::map<int, Precomputed>>, 0> getMappedReachIndices();
    std::vector<Point> getReachPath(int reachIndex) override;
    std::vector<Point> getMatchedSegment(int reachIndex);
    std::vector<Point> getMatchedPath(int reachIndex);

public:
    static std::shared_ptr<PreComputedReachNetwork> createFrom(const std::shared_ptr<ReachNetwork>& reachNetwork, const std::string& filePath);
    static std::shared_ptr<PreComputedReachNetwork> create();

private:
    static Point readPoint(std::ifstream& in);
    static std::vector<Point> readPoints(std::ifstream& in, int size);
};



#endif //PRECOMPUTEDREACHNETWORK_H
