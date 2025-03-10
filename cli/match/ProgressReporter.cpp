#include "ProgressReporter.h"

#include <iomanip>
#include <iostream>

ProgressReporter::ProgressReporter(const std::string& taskGroupName, int maxProgress)
    : m_taskGroupName(taskGroupName), m_taskLines(0), m_maxProgress(maxProgress) {
    std::cout << "Computing: " << taskGroupName << std::endl;
}

void ProgressReporter::createTask(const std::string& taskName) {
    m_tasks.emplace_back(taskName);
    m_taskProgress.emplace_back(-1);
    refresh();
}

void ProgressReporter::updateTaskProgress(const std::string& taskName, int progress) {
    for (auto i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i] == taskName) {
            m_taskProgress[i] = progress;
            break;
        }
    }
    refresh();
}

void ProgressReporter::finishTask(const std::string& taskName) {
    for (auto i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i] == taskName) {
            m_taskProgress[i] = m_maxProgress + 1;
            break;
        }
    }
    refresh();
}

void ProgressReporter::refresh() {
    if (m_taskLines > 0) {
        std::cout << "\033[" << m_taskLines << "A" << std::flush;
    }
    for (auto i = 0; i < m_tasks.size(); ++i) {
        std::cout << std::setw(40) << std::left << m_tasks[i];
        if (m_taskProgress[i] >= m_maxProgress) std::cout << std::setw(22) << ": completed" << std::endl;
        else if (m_taskProgress[i] < 0) std::cout << std::setw(22) << ": awaiting" << std::endl;
        else {
            int p = m_taskProgress[i] * 20 / m_maxProgress;
            if (p < 0) p = 0;
            if (p > 20) p = 20;
            std::cout << "[" << std::string(p, '=') << std::string(20 - p, ' ') << "]" << std::endl;
        }
    }
    m_taskLines = m_tasks.size();
}

void ProgressReporter::terminateAll() {
    if (m_taskLines > -1) {
        std::cout << "\033[" << m_taskLines + 1 << "A" << std::flush;
    }
    std::cout << "Completed: " << m_taskGroupName << std::endl;
    for (int i = 0; i < m_taskLines; i++) {
        std::cout << "\033[2K\r" << std::endl << std::flush;
    }
    if (m_taskLines > -1) {
        std::cout << "\033[" << m_taskLines << "A" << std::flush;
    }
}

