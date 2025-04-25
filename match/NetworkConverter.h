#ifndef NETWORKCONVERTER_H
#define NETWORKCONVERTER_H

#include <memory>

#include "ReachNetwork.h"

class NetworkConverter {
public:
    static std::shared_ptr<ReachNetwork> ng2rn(const std::shared_ptr<NetworkGraph>& networkGraph);
    static std::shared_ptr<NetworkGraph> rn2ng(const std::shared_ptr<ReachNetwork>& reachNetwork);
};



#endif //NETWORKCONVERTER_H
