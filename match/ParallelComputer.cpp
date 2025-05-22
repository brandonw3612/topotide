#include "ParallelComputer.h"

#include <thread>

void ParallelComputer::run(std::function<void()> task) {
    semaphore_.acquire();

    std::unique_lock lock(mutex_);
    ++activeTasks_;
    lock.unlock();

    std::thread([this, task = std::move(task)]() mutable {
        task();
        semaphore_.release();
        std::unique_lock<std::mutex> lock(mutex_);
        --activeTasks_;
        if (activeTasks_ == 0)
            cv_.notify_all();
    }).detach();
}

void ParallelComputer::waitAll() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&]() { return activeTasks_ == 0; });
}
