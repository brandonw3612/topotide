#ifndef REACHNETWORK_H
#define REACHNETWORK_H

#include <map>
#include <memory>
#include <vector>
#include <functional>

#include "Reach.h"

class PreComputedReachNetwork;

/// A network of reaches, connected via a hierarchical structure.
class ReachNetwork {
public:
    virtual ~ReachNetwork() = default;
    class Node;

private:
    class Edge;

public:
    enum Connection {
        Upstream = 0,
        Downstream = 1
    };

    enum FlowDirection {
        Forward = 0,
        Backward = 1,
        LikelyForward = 2,
        LikelyBackward = 3,
        Unknown = 256
    };

    /// Parent/Child relationship indicating the intersection point and the other node from a specific one.
    typedef class _ {
        Connection m_connection;
        Reach::IndexedPoint m_intersection;
        std::shared_ptr<Node> m_node;

    public:
        explicit _(Connection connection, const Reach::IndexedPoint& intersection, const std::shared_ptr<Node>& node);

        [[nodiscard]] Connection getConnection() const { return m_connection; }
        [[nodiscard]] Reach::IndexedPoint getIntersection() const { return m_intersection; }
        [[nodiscard]] std::shared_ptr<Node> getNode() const { return m_node; }
    } Parent, Child;

    /// A node in the reach network, representing a reach.
    class Node {
        /// The network this node belongs to. Used to access the parent/child relationships.
        std::shared_ptr<ReachNetwork> m_network;
        /// The depth of the node in the network.
        int m_depth;
        /// The reach associated with this node.
        std::shared_ptr<Reach> m_reach;
        /// Flow direction of the reach.
        FlowDirection m_flowDirection;

        /// Hidden constructor for creating a node. This constructor should only be called from the network.
        explicit Node(const std::shared_ptr<ReachNetwork>& network, const std::shared_ptr<Reach>& reach);

    public:
        /// Gets the depth of the node in the network.
        [[nodiscard]] int getDepth() const { return m_depth; }
        /// Gets the reach associated with this node.
        [[nodiscard]] std::shared_ptr<Reach> getReach() const { return m_reach; }

        /// Gets the upstream parent node of this node in the network.
        [[nodiscard]] std::shared_ptr<Parent> getUpstreamParent() const;
        /// Gets the downstream parent node of this node in the network.
        [[nodiscard]] std::shared_ptr<Parent> getDownstreamParent() const;
        /// Gets the child nodes of this node in the network.
        [[nodiscard]] std::vector<std::shared_ptr<Child>> getChildren() const;
        void updateFlowDirection();

    private:
        void findReasonableFlowDirection();

    public:
        struct DepthComparer {
            bool operator()(const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs) const {
                return lhs->getDepth() < rhs->getDepth();
            }
        };

        friend class ReachNetwork;
        friend class PreComputedReachNetwork;
    };


private:
    /// The REAL edge class representing the connection between two nodes in the network.
    class Edge {
        /// The global unique index of the parent node.
        int m_parentIndex;
        /// The global unique index of the child node.
        int m_childIndex;
        /// The intersection point between the two nodes.
        Reach::IndexedPoint m_intersection;

    public:
        /// Constructor for the edge class.
        explicit Edge(int parentIndex, int childIndex, const Reach::IndexedPoint& intersection)
            : m_parentIndex(parentIndex), m_childIndex(childIndex), m_intersection(intersection) {}

        /// Gets the index of the parent node.
        [[nodiscard]] int getParentIndex() const { return m_parentIndex; }
        /// Gets the index of the child node.
        [[nodiscard]] int getChildIndex() const { return m_childIndex; }
        /// Gets the intersection point between the two nodes.
        [[nodiscard]] Reach::IndexedPoint getIntersection() const { return m_intersection; }
    };

    /// The network itself, which contains all the nodes and edges. Used for assignments to nodes when creating them.
    std::shared_ptr<ReachNetwork> m_self;
    /// Nodes mapped by their unique index.
    std::map<int, std::shared_ptr<Node>> m_nodes;
    /// Upstream edges connecting the nodes in the network.
    std::vector<std::shared_ptr<Edge>> m_upstreamEdges;
    /// Downstream edges connecting the nodes in the network.
    std::vector<std::shared_ptr<Edge>> m_downstreamEdges;

    /// Hidden constructor for creating a reach network. This constructor should only be called from the static create() method.
    ReachNetwork() = default;
    /// Queries the upstream parent nodes of a specific node in the network. This function should only be called from the nodes.
    [[nodiscard]] std::shared_ptr<Parent> getUpstreamParent(int index);
    /// Queries the downstream parent nodes of a specific node in the network. This function should only be called from the nodes.
    [[nodiscard]] std::shared_ptr<Parent> getDownstreamParent(int index);
    /// Queries the child nodes of a specific node in the network. This function should only be called from the nodes.
    [[nodiscard]] std::vector<std::shared_ptr<Child>> getChildren(int index);

public:
    /// Creates a new Reach node in the network.
    std::shared_ptr<Node> createNode(const std::shared_ptr<Reach>& reach);
    /// Connects two nodes in the network with an edge.
    void addEdge(Connection connection, int parentIndex, int childIndex, const Reach::IndexedPoint& intersection);
    /// Gets the bounds of the network.
    [[nodiscard]] std::pair<Point, Point> getBounds() const;
    /// Gets all nodes in the network.
    [[nodiscard]] decltype(auto) getNodes() const { return m_nodes; }
    /// Filter the nodes in the network based on a predicate function.
    /// @return A new ReachNetwork instance containing only the nodes and connections that satisfy the predicate.
    std::shared_ptr<ReachNetwork> filter(const std::function<bool(const std::shared_ptr<Node>&)>& predicate);
    virtual std::vector<Point> getReachPath(int reachIndex);

public:
    /// Creates a new ReachNetwork instance.
    static std::shared_ptr<ReachNetwork> create();

    friend class PreComputedReachNetwork;
};

#endif //REACHNETWORK_H
