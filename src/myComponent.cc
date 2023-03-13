#include <sstream>

#include <sst/core/sst_config.h>
#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/componentInfo.h>
#include <sst/core/sst_types.h>
#include <sst/core/interfaces/stdMem.h>
#include <sst/core/interfaces/simpleMem.h>

#include "sst/elements/memHierarchy/memEvent.h"
#include "sst/elements/memHierarchy/memEventBase.h"
#include "sst/elements/merlin/router.h"
#include "myComponent.h"


MyComponent::MyComponent(SST::ComponentId_t id, SST::Params &params) : Component(id) {
    configureLinks();
}

void MyComponent::configureLinks() {
    SST::Link* link;
    link = configureLink("rtr_0", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
    rtr_0_Link = link;

    link = configureLink("rtr_1", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
    rtr_2_Link = link;

    link = configureLink("rtr_2", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
    rtr_1_Link = link;

    link = configureLink("rtr_3", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
    rtr_3_Link = link;

    link = configureLink("memory", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleMemoryEvent));
    memoryLink = link;
}

void MyComponent::init(unsigned int phase) {

}

void MyComponent::handleNetworkEvent(SST::Event* ev) {
    // Extract the encapsulated MemEvent
    out.output("handleNetworkEvent() received a RtrEvent event");
    memoryLink->send(ev);
}

void MyComponent::handleMemoryEvent(SST::Event *ev) {
    out.output("handleMemoryEvent() received a MemEventBase event");
    rtr_0_Link->send(ev);
}
