#include "Counters.h"

std::map<int, int> Counters::m_counters;

int Counters::next(int categoryId) {
    auto result = m_counters.find(categoryId);
    if (result == m_counters.end()) {
        m_counters[categoryId] = 0;
        return 0;
    }
    return ++result->second;
}

int Counters::nextReach() {
    return next(REACH_COUNTER);
}
