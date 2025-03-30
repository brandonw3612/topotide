#include "AbstractChannel.h"

AbstractChannel::AbstractChannel(const NetworkGraph::Edge& edge) {
    m_delta = edge.delta;
    m_points = edge.path;
    m_from = edge.from;
    m_to = edge.to;
    m_pointIDs = std::vector<int>();
    m_pointIDs.push_back(edge.from);
    m_pointIDs.push_back(edge.to);

    m_lowerBound = edge.path[0];
    m_upperBound = edge.path[0];
    updateBounds(edge);
}

bool AbstractChannel::appendEdge(const NetworkGraph::Edge &edge) {
    auto pathLength = (int) edge.path.size();
    if (m_from == edge.from) {
        for (int i = 1; i < pathLength; ++i) {
            m_points.insert(m_points.begin(), edge.path[i]);
        }
        m_from = edge.to;
        m_pointIDs.insert(m_pointIDs.begin(), edge.to);
        updateBounds(edge);
        return true;
    }
    if (m_from == edge.to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_points.insert(m_points.begin(), edge.path[i]);
        }
        m_from = edge.from;
        m_pointIDs.insert(m_pointIDs.begin(), edge.from);
        updateBounds(edge);
        return true;
    }
    if (m_to == edge.from) {
        for (int i = 1; i < pathLength; ++i) {
            m_points.push_back(edge.path[i]);
        }
        m_to = edge.to;
        m_pointIDs.push_back(edge.to);
        updateBounds(edge);
        return true;
    }
    if (m_to == edge.to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_points.push_back(edge.path[i]);
        }
        m_to = edge.from;
        m_pointIDs.push_back(edge.from);
        updateBounds(edge);
        return true;
    }
    return false;
}

bool AbstractChannel::mergeChannel(const std::shared_ptr<AbstractChannel> &other) {
    auto pathLength = (int) other->m_points.size();
    if (m_from == other->m_from) {
        for (int i = 1; i < pathLength; ++i) {
            m_points.insert(m_points.begin(), other->m_points[i]);
        }
        m_from = other->m_to;
        for (int i = 1; i < other->m_pointIDs.size(); ++i) {
            m_pointIDs.insert(m_pointIDs.begin(), other->m_pointIDs[i]);
        }
        updateBounds(other);
        return true;
    }
    if (m_from == other->m_to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_points.insert(m_points.begin(), other->m_points[i]);
        }
        m_from = other->m_from;
        for (int i = other->m_pointIDs.size() - 2; i >= 0; --i) {
            m_pointIDs.insert(m_pointIDs.begin(), other->m_pointIDs[i]);
        }
        updateBounds(other);
        return true;
    }
    if (m_to == other->m_from) {
        for (int i = 1; i < pathLength; ++i) {
            m_points.push_back(other->m_points[i]);
        }
        m_to = other->m_to;
        for (int i = 1; i < other->m_pointIDs.size(); ++i) {
            m_pointIDs.push_back(other->m_pointIDs[i]);
        }
        updateBounds(other);
        return true;
    }
    if (m_to == other->m_to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_points.push_back(other->m_points[i]);
        }
        m_to = other->m_from;
        for (int i = other->m_pointIDs.size() - 2; i >= 0; --i) {
            m_pointIDs.push_back(other->m_pointIDs[i]);
        }
        updateBounds(other);
        return true;
    }
    return false;
}

void AbstractChannel::updateBounds(const NetworkGraph::Edge &newEdge) {
    for (const auto &p: newEdge.path) {
        if (p.x < m_lowerBound.x) m_lowerBound.x = p.x;
        if (p.x > m_upperBound.x) m_upperBound.x = p.x;
        if (p.y < m_lowerBound.y) m_lowerBound.y = p.y;
        if (p.y > m_upperBound.y) m_upperBound.y = p.y;
    }
}

void AbstractChannel::updateBounds(const std::shared_ptr<AbstractChannel> &other) {
    if (other == nullptr) return;
    if (other->m_lowerBound.x < m_lowerBound.x) m_lowerBound.x = other->m_lowerBound.x;
    if (other->m_upperBound.x > m_upperBound.x) m_upperBound.x = other->m_upperBound.x;
    if (other->m_lowerBound.y < m_lowerBound.y) m_lowerBound.y = other->m_lowerBound.y;
    if (other->m_upperBound.y > m_upperBound.y) m_upperBound.y = other->m_upperBound.y;
}
