#ifndef SETSTACK_H
#define SETSTACK_H

#include <stack>
#include <unordered_set>

template<typename T>
class SetStack {
private:
    std::stack<T> m_stack;
    std::unordered_set<T> m_set;

public:
    bool push(const T& value) {
        if (m_set.contains(value)) return false;
        m_stack.push(value);
        m_set.insert(value);
        return true;
    }

    bool pop() {
        if (m_stack.empty()) return false;
        m_set.erase(m_stack.top());
        m_stack.pop();
        return true;
    }

    const T& top() const {
        if (m_stack.empty()) throw std::out_of_range("Stack is empty");
        return m_stack.top();
    }

    bool contains(const T& value) const {
        return m_set.contains(value);
    }

    void clear() {
        m_stack = std::stack<T>();
        m_set.clear();
    }

    [[nodiscard]] bool empty() const {
        return m_stack.empty();
    }

    [[nodiscard]] size_t size() const{
        return m_stack.size();
    }
};



#endif //SETSTACK_H
