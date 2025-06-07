#ifndef  VECTOR_UTILS_HPP
#define VECTOR_UTILS_HPP

#include <functional>
#include <vector>

class VectorUtils {
public:
    template <typename T, typename Predicate>
    static int firstIndexWhere(const std::vector<T>& vec, Predicate&& predicate) {
        for (int i = 0; i < vec.size(); ++i) {
            if (predicate(vec[i])) {
                return i;
            }
        }
        return -1; // Not found
    }

    template <typename T, typename Predicate>
    static int lastIndexWhere(const std::vector<T>& vec, Predicate&& predicate) {
        for (int i = vec.size() - 1; i >= 0; --i) {
            if (predicate(vec[i])) {
                return i;
            }
        }
        return -1; // Not found
    }

    template <typename T>
    static int firstIndexOf(const std::vector<T>& vec, const T& value) {
        for (int i = 0; i < vec.size(); ++i) {
            if (vec[i] == value) {
                return i;
            }
        }
        return -1; // Not found
    }

    template <typename T>
    static int lastIndexOf(const std::vector<T>& vec, const T& value) {
        for (int i = vec.size() - 1; i >= 0; --i) {
            if (vec[i] == value) {
                return i;
            }
        }
        return -1; // Not found
    }

    template <typename T, typename Predicate>
    static const T& firstWhere(const std::vector<T>& vec, Predicate&& predicate) {
        for (const auto& e : vec) {
            if (predicate(e)) {
                return e;
            }
        }
        throw std::runtime_error("No element found matching the predicate");
    }

    template <typename T, typename Predicate>
    static const T& lastWhere(const std::vector<T>& vec, Predicate&& predicate) {
        for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
            if (predicate(*it)) {
                return *it;
            }
        }
        throw std::runtime_error("No element found matching the predicate");
    }
};

#endif // VECTOR_UTILS_HPP
