#ifndef TOPOTIDE_ABSTRACTGRAPH_H
#define TOPOTIDE_ABSTRACTGRAPH_H

#include <map>
#include <memory>
#include <utility>
#include "AbstractChannel.h"

/// Abstract river channel graph.
class AbstractGraph {
public:

    /// Node in an AbstractGraph containing a channel its splitting/merging parent.
    class AGNode {
    private:
        std::shared_ptr<AbstractChannel> m_channel;
        std::shared_ptr<AGNode> m_splittingParent, m_mergingParent;
	    void setmatchedChannel(const std::shared_ptr<AbstractChannel>& channel){m_matchedChannel = channel;};
		std::shared_ptr<AbstractChannel> m_matchedChannel;
    public:
        /// Constructs a node with an AbstractChannel.
        /// @param channel The channel that current node represents in the graph.
        explicit AGNode(const std::shared_ptr<AbstractChannel>& channel) : m_channel(channel) {}

        /// Gets the channel entity of the current node.
        [[nodiscard]] const std::shared_ptr<AbstractChannel>& getChannel() const { return m_channel; }

        /// Gets the parent channel node from which the current node splits, if existent.
        [[nodiscard]] const std::shared_ptr<AGNode>& getSplittingParent() const { return m_splittingParent; }

        /// Gets the parent channel node into which the current node merges, if existent.
        [[nodiscard]] const std::shared_ptr<AGNode>& getMergingParent() const { return m_mergingParent; }

        /// Sets the parent channel node from which the current node splits.
        void setSplittingParent(const std::shared_ptr<AGNode>& parent) { m_splittingParent = parent; }

        /// Sets the parent channel node into which the current node merges.
        void setMergingParent(const std::shared_ptr<AGNode>& parent) { m_mergingParent = parent; }

        // Gets the main Channel of the graph
    	std::shared_ptr<AbstractChannel> getmainChannel() const;
    };

private:
    std::vector<std::shared_ptr<AGNode>> m_nodes;

public:
    /// Constructs an abstract graph with a list of nodes.
    AbstractGraph(const std::vector<std::shared_ptr<AGNode>>& nodes) : m_nodes(nodes) { }

public:
    /// Gets the node list.
    const std::vector<std::shared_ptr<AGNode>>& getNodes() const { return m_nodes; }

public:
    /// Gets the bound of the entire graph;
    std::pair<Point, Point> getBounds() const;
};


#endif //TOPOTIDE_ABSTRACTGRAPH_H
