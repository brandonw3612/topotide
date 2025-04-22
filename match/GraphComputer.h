#ifndef GRAPHCOMPUTER_H
#define GRAPHCOMPUTER_H

#include "ProgressReporter.h"

#include "RiverData.h"

class GraphComputer {
public:
    GraphComputer(std::string frameName, const std::shared_ptr<RiverData>& data, const std::shared_ptr<RiverFrame>& frame);

private:
    std::shared_ptr<RiverData> m_data;
    std::shared_ptr<RiverFrame> m_frame;

private:
    ProgressReporter m_pr;

public:
    void start();

private:
    void computeInputGraph();
    void computeInputDcel();
    void computeMsComplex();
    void computeMergeTree();
    void simplifyMsComplex();
    void msComplexToNetworkGraph();
};



#endif //GRAPHCOMPUTER_H
