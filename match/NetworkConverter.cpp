#include "NetworkConverter.h"

#include <cassert>
#include <ranges>
#include <unordered_set>
#include <unordered_map>

#include "NetworkGraphEdgeConnector.h"

std::shared_ptr<ReachNetwork> NetworkConverter::ng2rn(const std::shared_ptr<NetworkGraph> &networkGraph) {
    std::map<double, std::vector<NetworkGraph::Edge>, std::greater<>> deltaMap;
    for (int i = 0; i < networkGraph->edgeCount(); ++i) {
        if (!std::isfinite((*networkGraph)[networkGraph->edge(i).from].p.h) ||
            !std::isfinite((*networkGraph)[networkGraph->edge(i).to].p.h)) {
            continue;
        }
        auto delta = networkGraph->edge(i).delta;
        if (deltaMap.find(delta) == deltaMap.end()) {
            deltaMap[delta] = {networkGraph->edge(i)};
        } else {
            deltaMap[delta].push_back(networkGraph->edge(i));
        }
    }
    auto network = ReachNetwork::create();
    std::vector pc(networkGraph->vertexCount(), -1);
    for (const auto &p: deltaMap) {
        auto reaches = NetworkGraphEdgeConnector::connect(p.second);
        for (const auto &r: reaches) {
            auto node = network->createNode(r);
            if (pc[r->getFront().index] >= 0) {
                network->addEdge(
                    ReachNetwork::Connection::Upstream,
                    pc[r->getFront().index],
                    r->getIndex(),
                    {r->getFront().index, (*networkGraph)[r->getFront().index].p}
                );
            }
            if (pc[r->getBack().index] >= 0) {
                network->addEdge(
                    ReachNetwork::Connection::Downstream,
                    pc[r->getBack().index],
                    r->getIndex(),
                    {r->getBack().index, (*networkGraph)[r->getBack().index].p}
                );
            }
            for (const auto &ip: r->getIntersectionPoints()) {
                if (pc[ip.index] >= 0) continue;
                pc[ip.index] = r->getIndex();
            }
        }
    }
    for (auto &[_, node] : network->getNodes()) {
        node->updateFlowDirection();
    }
    return network;
}

std::shared_ptr<NetworkGraph> NetworkConverter::rn2ng(const std::shared_ptr<ReachNetwork> &reachNetwork) {
    auto networkGraph = std::make_shared<NetworkGraph>();
    std::unordered_map<int, Point> vertices;
    std::unordered_map<int, int> verticesRemap;
    // Get all vertices from the reach network.
    for (const auto & [_, node] : reachNetwork->getNodes()) {
        vertices[node->getReach()->getFront().index] = node->getReach()->getFront().location;
        vertices[node->getReach()->getBack().index] = node->getReach()->getBack().location;
    }
    // Add vertices to the network graph.
    for (const auto &p: vertices) {
        auto remapId = networkGraph->addVertex(p.second);
        verticesRemap[p.first] = remapId;
    }
    // Add edges to the network graph.
    for (const auto &[_, node] : reachNetwork->getNodes()) {
        auto reach = node->getReach();
        auto ips = reach->getIntersectionPoints();
        auto ipIndices = reach->getIpIndices();
        // We scan along the reach to only create edges between the intersection points that are vertices in the network graph.
        int prev = 0;
        while (prev < ips.size() - 1) {
            int next = prev + 1;
            while (next < ips.size() && !vertices.contains(ips[next].index)) {
                ++next;
            }
            auto path = std::vector(reach->getPoints().begin() + ipIndices[prev],
                reach->getPoints().begin() + ipIndices[next] + 1);
            networkGraph->addEdge(verticesRemap[ips[prev].index], verticesRemap[ips[next].index], path, reach->getDelta());
            prev = next;
        }
    }
    return networkGraph;
}
