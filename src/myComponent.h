#ifndef _MYCOMPONENT_H
#define _MYCOMPONENT_H

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/interfaces/simpleMem.h>
#include <sst/core/interfaces/simpleNetwork.h>

class MyComponent : public SST::Component {
public:
    MyComponent(SST::ComponentId_t id, SST::Params& params);
    void init(unsigned int phase);
    void setup();
    void finish();

private:
    void handleMemoryEvent(SST::Interfaces::SimpleMem::Request *ev);
    void handleNetworkEvent(SST::Interfaces::SimpleNetwork::Request *ev);

    void convertToStandardMemory(SST::Interfaces::SimpleNetwork::Request *req);
    void convertToSimpleNetwork(SST::Interfaces::StandardMem::Request *req);

    SST_ELI_REGISTER_COMPONENT(
            MyComponent,
    "myComponent",
    "MyComponent",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "A component that converts between SST::Interfaces::StandardMemory and SST::Interfaces::SimpleNetwork",
    COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
    { "memory_num_vc", "Number of virtual channels for memory interface", "2" },
    { "network_num_vc", "Number of virtual channels for network interface", "2" }
    )

    SST_ELI_DOCUMENT_PORTS(
    {"memory", "Memory interface", {"simpleMem.Event", ""}},
    {"network", "Network interface", {"simpleNetwork.Request", ""}}
    )

    SST::Output out;
    SST::Link* memoryLink;
    SST::Link* networkLink;
};

#endif /* _MYCOMPONENT_H */