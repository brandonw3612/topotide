#ifndef SVG_H
#define SVG_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "ReachNetwork.h"
#include "PreComputedReachNetwork.h"

class SVG {
    public:
    SVG(const std::string &outputPrefix, const std::string &beforeYear, const std::string &afterYear,
                            double sourceDeltaThreshold, double targetDeltaThreshold, 
                            const std::shared_ptr<PreComputedReachNetwork>& matching);
    void createMatchingOutputs(bool includeLabels);

    private:
    int m_width;
    int m_height;
    std::string m_outputPrefix;
    std::string m_beforeYear;
    std::string m_afterYear;
    double m_sourceDeltaThreshold;
    double m_targetDeltaThreshold;
    std::shared_ptr<PreComputedReachNetwork> m_matching;

    
    std::vector<std::string> m_colors = {"#f6eff7","#d0d1e6","#a6bddb","#67a9cf","#3690c0","#02818a","#016450"}; // From colorbrewer
    
    void drawPath(std::ofstream& ofs, const std::vector<Point>& path, std::string color);
    void drawLegend(std::ofstream& ofs, std::map<double, std::string> colorAssignments);
    std::map<double, std::string> getColorAssignments();

};


#endif // SVG_H