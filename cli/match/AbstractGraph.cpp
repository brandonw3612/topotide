#include "AbstractGraph.h"

const std::shared_ptr<AbstractGraph::AGNode> &AbstractGraph::getRoot() const {
    return m_root;
}
