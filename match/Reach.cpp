#include "Reach.h"

#include "Counters.h"

Reach::Builder::Builder(const NetworkGraph::Edge &edge) {
    m_reach = std::make_shared<Reach>();

    m_reach->m_delta = edge.delta;
    m_reach->m_points = edge.path;

    m_reach->m_intersectionPoints.emplace_back(edge.from, edge.path.front());
    m_reach->m_ipIndices.emplace_back(0);
    m_reach->m_intersectionPoints.emplace_back(edge.to, edge.path.back());
    m_reach->m_ipIndices.emplace_back(static_cast<int>(edge.path.size()) - 1);

    m_reach->m_lowerBound = edge.path[0];
    m_reach->m_upperBound = edge.path[0];
    updateBounds(edge);
}

bool Reach::Builder::appendEdge(const NetworkGraph::Edge &edge) const {
    auto pathLength = static_cast<int>(edge.path.size());
    if (getFront().index == edge.from) {
        for (int i = 1; i < pathLength; ++i) {
            prepend(m_reach->m_points, edge.path[i]);
        }
        prepend(m_reach->m_intersectionPoints, {edge.to, edge.path.back()});
        prepend(m_reach->m_ipIndices, m_reach->m_ipIndices.front() - pathLength + 1);
        updateBounds(edge);
        return true;
    }
    if (getFront().index == edge.to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            prepend(m_reach->m_points, edge.path[i]);
        }
        prepend(m_reach->m_intersectionPoints, {edge.from, edge.path.front()});
        prepend(m_reach->m_ipIndices, m_reach->m_ipIndices.front() - pathLength + 1);
        updateBounds(edge);
        return true;
    }
    if (getBack().index == edge.from) {
        for (int i = 1; i < pathLength; ++i) {
            m_reach->m_points.push_back(edge.path[i]);
        }
        m_reach->m_intersectionPoints.emplace_back(edge.to, edge.path.back());
        m_reach->m_ipIndices.emplace_back(m_reach->m_ipIndices.back() + pathLength - 1);
        updateBounds(edge);
        return true;
    }
    if (getBack().index == edge.to) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_reach->m_points.push_back(edge.path[i]);
        }
        m_reach->m_intersectionPoints.emplace_back(edge.from, edge.path.front());
        m_reach->m_ipIndices.emplace_back(m_reach->m_ipIndices.back() + pathLength - 1);
        updateBounds(edge);
        return true;
    }
    return false;
}

bool Reach::Builder::pendReach(const std::shared_ptr<Builder> &other) const {
    auto pathLength = static_cast<int>(other->m_reach->m_points.size());
    auto labeledPointLength = static_cast<int>(other->m_reach->m_intersectionPoints.size());
    if (getFront().index == other->getFront().index) {
        for (int i = 1; i < pathLength; ++i) {
            prepend(m_reach->m_points, other->m_reach->m_points[i]);
        }
        auto indexOffset = m_reach->m_ipIndices.front() + other->m_reach->m_ipIndices.front();
        for (int i = 1; i < labeledPointLength; ++i) {
            prepend(m_reach->m_intersectionPoints, other->m_reach->m_intersectionPoints[i]);
            prepend(m_reach->m_ipIndices, -other->m_reach->m_ipIndices[i] + indexOffset);
        }
        updateBounds(other);
        return true;
    }
    if (getFront().index == other->getBack().index) {
        for (int i = pathLength - 2; i >= 0; --i) {
            prepend(m_reach->m_points, other->m_reach->m_points[i]);
        }
        auto indexOffset = m_reach->m_ipIndices.front() - other->m_reach->m_ipIndices.back();
        for (int i = labeledPointLength - 2; i >= 0; --i) {
            prepend(m_reach->m_intersectionPoints, other->m_reach->m_intersectionPoints[i]);
            prepend(m_reach->m_ipIndices, other->m_reach->m_ipIndices[i] + indexOffset);
        }
        updateBounds(other);
        return true;
    }
    if (getBack().index == other->getFront().index) {
        for (int i = 1; i < pathLength; ++i) {
            m_reach->m_points.push_back(other->m_reach->m_points[i]);
        }
        auto indexOffset = m_reach->m_ipIndices.back() - other->m_reach->m_ipIndices.front();
        for (int i = 1; i < labeledPointLength; ++i) {
            m_reach->m_intersectionPoints.push_back(other->m_reach->m_intersectionPoints[i]);
            m_reach->m_ipIndices.push_back(other->m_reach->m_ipIndices[i] + indexOffset);
        }
        updateBounds(other);
        return true;
    }
    if (getBack().index == other->getBack().index) {
        for (int i = pathLength - 2; i >= 0; --i) {
            m_reach->m_points.push_back(other->m_reach->m_points[i]);
        }
        auto indexOffset = m_reach->m_ipIndices.back() + other->m_reach->m_ipIndices.back();
        for (int i = labeledPointLength - 2; i >= 0; --i) {
            m_reach->m_intersectionPoints.push_back(other->m_reach->m_intersectionPoints[i]);
            m_reach->m_ipIndices.push_back(-other->m_reach->m_ipIndices[i] + indexOffset);
        }
        updateBounds(other);
        return true;
    }
    return false;
}

std::shared_ptr<Reach> Reach::Builder::build() {
    m_reach->index = Counters::nextReach();
    auto ipOffset = m_reach->m_ipIndices.front();
    for (int i = 0; i < m_reach->m_ipIndices.size(); ++i) {
        m_reach->m_ipIndices[i] -= ipOffset;
    }
    return m_reach;
}

void Reach::Builder::updateBounds(const NetworkGraph::Edge &newEdge) const {
    for (const auto &p: newEdge.path) {
        if (p.x < m_reach->m_lowerBound.x) m_reach->m_lowerBound.x = p.x;
        if (p.x > m_reach->m_upperBound.x) m_reach->m_upperBound.x = p.x;
        if (p.y < m_reach->m_lowerBound.y) m_reach->m_lowerBound.y = p.y;
        if (p.y > m_reach->m_upperBound.y) m_reach->m_upperBound.y = p.y;
    }
}

void Reach::Builder::updateBounds(const std::shared_ptr<Builder> &other) const {
    if (other == nullptr) return;
    if (other->m_reach->m_lowerBound.x < m_reach->m_lowerBound.x)
        m_reach->m_lowerBound.x = other->m_reach->m_lowerBound.x;
    if (other->m_reach->m_upperBound.x > m_reach->m_upperBound.x)
        m_reach->m_upperBound.x = other->m_reach->m_upperBound.x;
    if (other->m_reach->m_lowerBound.y < m_reach->m_lowerBound.y)
        m_reach->m_lowerBound.y = other->m_reach->m_lowerBound.y;
    if (other->m_reach->m_upperBound.y > m_reach->m_upperBound.y)
        m_reach->m_upperBound.y = other->m_reach->m_upperBound.y;
}

int Reach::findIndexedPoint(int index) const {
    for (int i = 0; i < m_intersectionPoints.size(); ++i) {
        if (m_intersectionPoints[i].index == index) return m_ipIndices[i];
    }
    return -1;
}
