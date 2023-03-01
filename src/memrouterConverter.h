//
// Created by tokth on 3/1/23.
//

#ifndef MEMRTRCONVERTER_H
#define MEMRTRCONVERTERT_H

#include <sst/core/component.h>
#include <sst/core/interfaces/simpleNetwork.h>
#include <sst/core/interfaces/stdMem.h>
#include <sst/core/output.h>
#include "sst/elements/memHierarchy/memEvent.h"

using namespace SST;
using namespace SST::MemHierarchy;

class MemRouterConverter : public SST::Component {
public:
    MemRouterConverter(SST::ComponentId_t id, SST::Params& params);

    void init(unsigned int phase) override;
    void setup() override;
    void finish() override;

    bool tick(SST::Cycle_t cycle);

    SST_ELI_REGISTER_COMPONENT(
            MemRouterConverter,
    "memrouterConverter",
    "MemRouterConverter",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "Example component for SST",
    COMPONENT_CATEGORY_UNCATEGORIZED
    )

// Document the parameters
    SST_ELI_DOCUMENT_PARAMS(
    { "network_num_vc", "Number of virtual channels to use for the SimpleNetwork interface", "1" },
    { "network_port_name", "Name of the SimpleNetwork port", "network" },
    { "memory_port_name", "Name of the StandardMemory port", "memory" }
    )

    // Port callback function
    SST::Port* providePort(const std::string& portName, int portIndex, SST::Port::PortFlags flags) override;

private:
    void convertToStandardMemory(SST::Event* event, uint32_t fromNetworkAddr, Command cmd);
    void convertToSimpleNetwork(SST::Event* event, MemEvent* memEvent);

    SST::Output output;
    SST::Interfaces::SimpleNetwork* simpleNetwork;
    SST::Interfaces::StandardMem* standardMemory;

    int network_num_vc;

};

#endif /* MEMRTRCONVERTER_H */
