#include "GraphComputer.h"

#include "mscomplexcreator.h"
#include "mscomplexsimplifier.h"
#include "mstonetworkgraphcreator.h"

GraphComputer::GraphComputer(
    std::string frameName,
    const std::shared_ptr<RiverData>& data,
    const std::shared_ptr<RiverFrame>& frame)
    : m_data(data), m_frame(frame), m_pr(frameName) {}

void GraphComputer::start() {
	computeInputGraph();
	computeInputDcel();
	computeMsComplex();
	simplifyMsComplex();
	msComplexToNetworkGraph();
	m_pr.terminateAll();
}


void GraphComputer::computeInputGraph() {
	m_pr.createTask("Computing input graph");
	auto inputGraph = std::make_shared<InputGraph>(
	                           m_frame->m_heightMap,
	                           m_data->boundaryRasterized());
	m_pr.updateTaskProgress("Computing input graph", 100);
	{
		QWriteLocker lock(&(m_frame->m_inputGraphLock));
		m_frame->m_inputGraph = inputGraph;
	}
	m_pr.finishTask("Computing input graph");
}

void GraphComputer::computeInputDcel() {
	m_pr.createTask("Computing input DCEL");
	auto inputDcel = std::make_shared<InputDcel>(*m_frame->m_inputGraph);
	m_pr.updateTaskProgress("Computing input DCEL", 100);
	{
		QWriteLocker lock(&(m_frame->m_inputDcelLock));
		m_frame->m_inputDcel = inputDcel;
	}
	m_pr.finishTask("Computing input DCEL");
}

void GraphComputer::computeMsComplex() {
	m_pr.createTask("Computing MS complex");
	auto msComplex = std::make_shared<MsComplex>();
	MsComplexCreator msCreator(m_frame->m_inputDcel,
	                           msComplex, [this](int progress) {
		m_pr.updateTaskProgress("Computing MS complex", progress);
	});
	msCreator.create();
	{
		QWriteLocker lock(&(m_frame->m_msComplexLock));
		m_frame->m_msComplex = msComplex;
	}
	m_pr.finishTask("Computing MS complex");
}

void GraphComputer::simplifyMsComplex() {
	m_pr.createTask("Simplifying MS complex");
	auto msSimplified = std::make_shared<MsComplex>(*m_frame->m_msComplex);
	MsComplexSimplifier msSimplifier(msSimplified,
	                                 [this](int progress) {
		m_pr.updateTaskProgress("Simplifying MS complex", progress);
	});
	msSimplifier.simplify();
	m_pr.finishTask("Simplifying MS complex");

	m_pr.createTask("Compacting MS complex");

	msSimplified->compact();
	{
		QWriteLocker lock(&(m_frame->m_msComplexLock));
		m_frame->m_msComplex = msSimplified;
	}
	m_pr.finishTask("Compacting MS complex");
}

void GraphComputer::msComplexToNetworkGraph() {
	m_pr.createTask("Converting MS complex into network");
	auto networkGraph = std::make_shared<NetworkGraph>();
	MsToNetworkGraphCreator networkGraphCreator(
	            m_frame->m_msComplex,
	            networkGraph,
	            [this](int progress) {
		m_pr.updateTaskProgress("Converting MS complex into network", progress);
	});
	networkGraphCreator.create();
	{
		QWriteLocker lock(&(m_frame->m_networkGraphLock));
		m_frame->m_networkGraph = networkGraph;
	}
	m_pr.finishTask("Converting MS complex into network");
}