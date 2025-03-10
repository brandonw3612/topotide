#ifndef PROGRESSREPORTER_H
#define PROGRESSREPORTER_H

#include <string>
#include <vector>

class ProgressReporter {
private:
    std::string m_taskGroupName;
    std::vector<std::string> m_tasks;
    std::vector<int> m_taskProgress;
    int m_maxProgress;
    int m_taskLines;

public:
    explicit ProgressReporter(const std::string& taskGroupName, int maxProgress = 100);

    void createTask(const std::string& taskName);
    void updateTaskProgress(const std::string& taskName, int progress);
    void finishTask(const std::string& taskName);
    void terminateAll();

private:
    void refresh();
};



#endif //PROGRESSREPORTER_H
