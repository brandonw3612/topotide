#ifndef PARALLELCOMPUTER_H
#define PARALLELCOMPUTER_H

#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <functional>

class ParallelComputer {
public:
    explicit ParallelComputer(int maxConcurrency)
        : semaphore_(maxConcurrency), maxConcurrency_(maxConcurrency) {}

    void run(std::function<void()> task);

    void waitAll();

private:
    std::counting_semaphore<1024> semaphore_;
    int maxConcurrency_;
    std::mutex mutex_;
    std::condition_variable cv_;
    int activeTasks_ = 0;
};

#endif //PARALLELCOMPUTER_H
