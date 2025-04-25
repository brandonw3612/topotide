#ifndef VECTORSTACK_H
#define VECTORSTACK_H

#include <vector>

template <typename T>
class VectorStack {
private:
    std::vector<T> m_data;

public:
    void push(const T& value) {
        m_data.push_back(value);
    }

    void pop() {
        if (m_data.empty()) throw std::out_of_range("Stack is empty");
        m_data.pop_back();
    }

    const T& top() const {
        if (m_data.empty()) throw std::out_of_range("Stack is empty");
        return m_data.back();
    }

    void clear() {
        m_data.clear();
    }

    [[nodiscard]] bool empty() const {
        return m_data.empty();
    }

    [[nodiscard]] size_t size() const {
        return m_data.size();
    }

    const std::vector<T>& asVector() const {
        return m_data;
    }
};



#endif //VECTORSTACK_H
