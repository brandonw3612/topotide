#include "SVG.h"

#include <format>
#include <iostream>
#include <algorithm>
#include <set>

SVG::SVG(const std::string &outputPrefix, const std::string &beforeYear, const std::string &afterYear,
        double sourceDeltaThreshold, double targetDeltaThreshold, const std::shared_ptr<PreComputedReachNetwork> &matching) {
    m_width = 1000;
    m_height = 500;
    m_scale = 4;
    m_outputPrefix = outputPrefix;
    m_beforeYear = beforeYear;
    m_afterYear = afterYear;
    m_sourceDeltaThreshold = sourceDeltaThreshold;
    m_targetDeltaThreshold = targetDeltaThreshold;
    m_matching = matching;
}

void SVG::createMatchingOutputs(bool includeLabels) {
    std::string outputPathBefore;
    std::string outputPathAfter;
    if (includeLabels) {
        outputPathBefore = std::format("{}Before{}-{}_{}_{}_labelled.svg", m_outputPrefix, m_beforeYear, m_afterYear, m_sourceDeltaThreshold, m_targetDeltaThreshold);
        outputPathAfter = std::format("{}After{}-{}_{}_{}_labelled.svg", m_outputPrefix, m_beforeYear, m_afterYear, m_sourceDeltaThreshold, m_targetDeltaThreshold);
    } else {
        outputPathBefore = std::format("{}Before{}-{}_{}_{}_unlabelled.svg", m_outputPrefix, m_beforeYear, m_afterYear, m_sourceDeltaThreshold, m_targetDeltaThreshold);
        outputPathAfter = std::format("{}After{}-{}_{}_{}_unlabelled.svg", m_outputPrefix, m_beforeYear, m_afterYear, m_sourceDeltaThreshold, m_targetDeltaThreshold);
    }
    

    std::ofstream ofsBefore(outputPathBefore);
    std::ofstream ofsAfter(outputPathAfter);
    std::string SVGInitString = std::format("<svg xmlns='http://www.w3.org/2000/svg' width='{}' height='{}' viewBox='0 0 {} {}'>\n",
                        m_width * m_scale,
                        m_height * m_scale,
                        m_width * m_scale,
                        m_height * m_scale);
    ofsBefore << SVGInitString;
    ofsAfter << SVGInitString;

    auto colorAssignments = getColorAssignments();

    for (const auto& id : m_matching->getMappedReachIndices()) {
        auto node = m_matching->getNodes()[id];
        auto beforeReach = node->getReach();
        auto delta = beforeReach->getDelta();
        auto afterMatchedSegment = m_matching->getMatchedSegment(id);
        auto color = colorAssignments[delta];
        drawPath(ofsBefore, beforeReach->getPoints(), color);
        drawPath(ofsAfter, afterMatchedSegment, color);
    }

    if (includeLabels) {
        std::string labelGroupStart = std::format("<g id='labels' font-size='{}' fill='black'>\n", 12 * m_scale);
        ofsBefore << labelGroupStart;
        ofsAfter << labelGroupStart;

        auto pointComp = [](const Point& p1, const Point& p2) {
            if (p1.x < p2.x) return true;
            if (p1.x > p2.x) return false;
            if (p1.y < p2.y) return true;
            if (p1.y > p2.y) return false;
            return p1.h < p2.h;
        };
        auto pointPairComp = [pointComp](const std::pair<Point, Point>& pp1, const std::pair<Point, Point>& pp2) {
            if (pointComp(pp1.first, pp2.first)) return true;
            if (pointComp(pp2.first, pp1.first)) return false;
            return pointComp(pp1.second, pp2.second);
        };
        std::set<std::pair<Point, Point>, decltype(pointPairComp)> vertexMatching(pointPairComp);
        for (const auto& id : m_matching->getMappedReachIndices()) {
            auto node = m_matching->getNodes()[id];
            auto beforeSegment = node->getReach()->getPoints();
            auto afterMatchedSegment = m_matching->getMatchedSegment(id);
            if (afterMatchedSegment.size()) {
                vertexMatching.insert({beforeSegment[0], afterMatchedSegment[0]});
                vertexMatching.insert({beforeSegment.back(), afterMatchedSegment.back()});
            } else {
                vertexMatching.insert({beforeSegment[0], Point(-1, -1, -1)});
                vertexMatching.insert({beforeSegment.back(), Point(-1, -1, -1)});
            }
            
        }
        int count = 0;
        for (auto& vertexMatch : vertexMatching) {
            auto& pointBefore = vertexMatch.first;
            auto& pointAfter = vertexMatch.second;
            ofsBefore << std::format("<text x='{}' y='{}'>{}</text>\n", pointBefore.x * m_scale, pointBefore.y * m_scale, count);
            if (pointAfter.x != -1) {
                ofsAfter << std::format("<text x='{}' y='{}'>{}</text>\n", pointAfter.x * m_scale, pointAfter.y * m_scale, count);
            }
            count++;
        }
        ofsBefore << "</g>\n";
        ofsAfter << "</g>\n";
    }

    drawLegend(ofsBefore, colorAssignments);
    drawLegend(ofsAfter, colorAssignments);

    ofsBefore << "</svg>" << std::endl;
    ofsBefore.close();
    ofsAfter << "</svg>" << std::endl;
    ofsAfter.close();
}

void SVG::createBlackAfterNetworkOutput(std::shared_ptr<ReachNetwork> &network) {
    std::string outputPath = std::format("{}After{}-{}_{}_{}_black.svg", m_outputPrefix, m_beforeYear, m_afterYear, m_sourceDeltaThreshold, m_targetDeltaThreshold);
    std::ofstream ofs(outputPath);
    std::string SVGInitString = std::format("<svg xmlns='http://www.w3.org/2000/svg' width='{}' height='{}' viewBox='0 0 {} {}'>\n",
                        m_width * m_scale,
                        m_height * m_scale,
                        m_width * m_scale,
                        m_height * m_scale);
    ofs << SVGInitString;
    for (auto& [id, node] : network->getNodes()) {
        drawPath(ofs, node->getReach()->getPoints(), "black");
    }
    ofs << "</svg>" << std::endl;
    ofs.close();
}

void SVG::drawPath(std::ofstream &ofs, const std::vector<Point> &path, std::string color) {
    ofs << std::format("<g stroke='{}' stroke-width='2'>\n", color);  
    for (int i = 0; i < (int) path.size() - 1; ++i) {
        ofs << std::format("<line x1='{:.0f}' y1='{:.0f}' x2='{:.0f}' y2='{:.0f}' />\n", path[i].x * m_scale,
                                                                        path[i].y  * m_scale,
                                                                        path[i + 1].x * m_scale,
                                                                        path[i + 1].y * m_scale);
    }
    ofs << "</g>\n";
}

void SVG::drawLegend(std::ofstream& ofs, std::map<double, std::string> colorAssignments) {
    int boxHeight =  25 * m_colors.size() + 10 + 20;
    ofs << std::format("<g id='legend' transform='translate({}, {})'>\n", m_width * 0.7 * m_scale, m_height * 0.58 * m_scale);
    ofs << std::format("<rect x='0' y='0' width='{}' height='{}' fill='white' stroke='#000' stroke-width='0.5'/>\n", 180 * m_scale, boxHeight * m_scale);
    

    std::map<std::string, std::pair<double, double>> colorToDeltaRange;
    for (auto& [delta, color] : colorAssignments) {
        if (colorToDeltaRange.count(color)) {
            colorToDeltaRange[color].second = delta;
        } else {
            colorToDeltaRange[color] = {delta, delta};
        }
    }
    
    int count = 0;
    double prevHighest = 0;
    ofs << std::format("<text x='{}' y='{}' font-size='{}' fill='black'>{}</text>\n", 10 * m_scale, 20 * m_scale, 14 * m_scale, "&#948;-value of reaches");
    for (auto it = colorToDeltaRange.rbegin(); it != colorToDeltaRange.rend(); it++) {
        auto& color = it->first;
        auto& deltaRange = it->second;

        std::string deltaRangeString;
        if (prevHighest == 0) {
            deltaRangeString = std::format("&lt;= {:.1e}", deltaRange.second);
        } else if (deltaRange.first == std::numeric_limits<double>::infinity()) {
            deltaRangeString = std::format("> {:.1e}", prevHighest);
        } else {
            deltaRangeString = std::format("{:.1e}-{:.1e}", prevHighest, deltaRange.second);
        }
        prevHighest = deltaRange.second;
    

        ofs << std::format("<rect x='{}' y='{}' width='{}' height='{}' fill='{}'/>\n", 10 * m_scale, (boxHeight - (25 + 25 * count)) * m_scale, 20 * m_scale, 20 * m_scale, color);
        ofs << std::format("<text x='{}' y='{}' font-size='{}' fill='black'>{}</text>\n", 40 * m_scale, (boxHeight - (10 + 25 * count)) * m_scale, 12 * m_scale, deltaRangeString);

        count++;
    }

    ofs << "</g>\n";
}

std::map<double, std::string> SVG::getColorAssignments() {
    std::map<double, std::string> colorAssignments;
    std::vector<double> deltaValues;
    for (const auto& id : m_matching->getMappedReachIndices()) {
        auto node = m_matching->getNodes()[id];
        auto delta = node->getReach()->getDelta();
        deltaValues.push_back(delta);
    }
    std::sort(deltaValues.begin(), deltaValues.end());
    deltaValues.erase(unique(deltaValues.begin(), deltaValues.end()), deltaValues.end());
    colorAssignments[deltaValues.back()] = m_colors.back();
    deltaValues.pop_back();

    int valuesCount = deltaValues.size();
    int colorsCount = m_colors.size() - 1;
    int valuesPerColor = valuesCount / colorsCount;
    if (valuesCount % colorsCount) valuesPerColor++;
    for (int i = 0; i < deltaValues.size(); i++) {
        colorAssignments[deltaValues[i]] = m_colors[i / valuesPerColor];
    }
    return colorAssignments;
}