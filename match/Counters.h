#ifndef COUNTERS_H
#define COUNTERS_H

#define REACH_COUNTER 1111

#include <map>

class Counters {
private:
    static std::map<int, int> m_counters;

private:
    static int next(int categoryId);

public:
    static int nextReach();
};



#endif //COUNTERS_H
