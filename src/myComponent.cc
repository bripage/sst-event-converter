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

    link = configureLink("memory", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleMemoryEvent));
    memoryLink = link;
}

void MyComponent::init(unsigned int phase) {
    if (!rtr_0_Link)
        out.fatal(CALL_INFO, -1, "%s, Error: unable to configure link on rtr_0_Link\n", getName().c_str());

    if (!memoryLink)
        out.fatal(CALL_INFO, -1, "%s, Error: unable to configure link memoryLink'\n", getName().c_str());
}

void MyComponent::handleNetworkEvent(SST::Event* ev) {
    // Extract the encapsulated MemEvent
    //out.fatal(CALL_INFO, -1, "%s, received something on port rtr_0\n", getName().c_str());
    memoryLink->send(ev);
}

void MyComponent::handleMemoryEvent(SST::Event *ev) {
    //out.fatal(CALL_INFO, -1, "%s, received somethign on port memory\n", getName().c_str());
    rtr_0_Link->send(ev);
}
