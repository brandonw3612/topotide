#ifndef TOPOTIDE_ABSTRACTGRAPH_H
#define TOPOTIDE_ABSTRACTGRAPH_H

#include <map>
#include <memory>
#include <utility>
#include "AbstractChannel.h"

class AbstractGraph {
public:
    class AGNode {
    private:
        std::shared_ptr<AbstractChannel> m_channel;
        std::shared_ptr<AGNode> m_splittingParent, m_mergingParent;
        std::map<double, std::shared_ptr<AGNode>, std::greater<>> m_children;

    public:
        explicit AGNode(const std::shared_ptr<AbstractChannel>& channel) : m_channel(channel) {}
        [[nodiscard]] const std::shared_ptr<AbstractChannel>& getChannel() const { return m_channel; }
        [[nodiscard]] const std::shared_ptr<AGNode>& getSplittingParent() const { return m_splittingParent; }
        [[nodiscard]] const std::shared_ptr<AGNode>& getMergingParent() const { return m_mergingParent; }
        [[nodiscard]] const std::map<double, std::shared_ptr<AGNode>, std::greater<>>& getChildren() const { return m_children; }
        void setSplittingParent(const std::shared_ptr<AGNode>& parent) { m_splittingParent = parent; }
        void setMergingParent(const std::shared_ptr<AGNode>& parent) { m_mergingParent = parent; }
        void addChild(double delta, const std::shared_ptr<AGNode>& child) { m_children[delta] = child; }
    };

private:
    std::shared_ptr<AGNode> m_root;

public:
    explicit AbstractGraph(const std::shared_ptr<AGNode>& rootNode) : m_root(rootNode) {}

    const std::shared_ptr<AGNode>& getRoot() const;
};


#endif //TOPOTIDE_ABSTRACTGRAPH_H
