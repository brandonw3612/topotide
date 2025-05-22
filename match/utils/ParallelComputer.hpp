#ifndef PARALLELCOMPUTER_H
#define PARALLELCOMPUTER_H

#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

class ParallelComputer {
public:
    explicit ParallelComputer(int maxConcurrency)
        : semaphore_(maxConcurrency), maxConcurrency_(maxConcurrency) {}

    template <typename Func, typename... Args>
    auto run(Func&& func, Args&&... args)
        -> std::future<std::invoke_result_t<Func, Args...>> {

        using ReturnType = std::invoke_result_t<Func, Args...>;
        auto taskPack = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        );

        std::future<ReturnType> future = taskPack->get_future();

        semaphore_.acquire();

        {
            std::unique_lock<std::mutex> lock(mutex_);
            ++activeTasks_;
        }

        std::thread([this, taskPack]() {
            (*taskPack)();

            {
                std::unique_lock<std::mutex> lock(mutex_);
                --activeTasks_;
                if (activeTasks_ == 0)
                    cv_.notify_all();
            }

            semaphore_.release();
        }).detach();

        return future;
    }

    void waitAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return activeTasks_ == 0; });
    }

private:
    std::counting_semaphore<1024> semaphore_;
    int maxConcurrency_;
    std::mutex mutex_;
    std::condition_variable cv_;
    int activeTasks_ = 0;
};

#endif //PARALLELCOMPUTER_H
