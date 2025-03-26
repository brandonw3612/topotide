#include "AbstractChannel.h"

AbstractChannel::AbstractChannel(const NetworkGraph::Edge& edge) {
    m_points = edge.path;
    m_from = edge.from;
    m_to = edge.to;
    m_pointIDs = std::vector<int>();
    m_pointIDs.push_back(edge.from);
    m_pointIDs.push_back(edge.to);
}

bool AbstractChannel::appendEdge(const NetworkGraph::Edge &edge) {
    auto pathLength = (int) edge.path.size();
    if (m_from == edge.from) {
        for (int i = 1; i < pathLength; ++i) {
            m_points.insert(m_points.begin(), edge.path[i]);
        }
        m_from = edge.to;
        m_pointIDs.insert(m_pointIDs.begin(), edge.to);
        return true;
    }
    if (m_from == edge.to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_points.insert(m_points.begin(), edge.path[i]);
        }
        m_from = edge.from;
        m_pointIDs.insert(m_pointIDs.begin(), edge.from);
        return true;
    }
    if (m_to == edge.from) {
        for (int i = 1; i < pathLength; ++i) {
            m_points.push_back(edge.path[i]);
        }
        m_to = edge.to;
        m_pointIDs.push_back(edge.to);
        return true;
    }
    if (m_to == edge.to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_points.push_back(edge.path[i]);
        }
        m_to = edge.from;
        m_pointIDs.push_back(edge.from);
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
        return true;
    }
    return false;
}

int AbstractChannel::getLength() const {
    return m_points.size();
}

const std::vector<int> &AbstractChannel::getPointIDs() const {
    return m_pointIDs;
}

int AbstractChannel::getStartPointID() const {
    return m_from;
}

int AbstractChannel::getEndPointID() const {
    return m_to;
}
