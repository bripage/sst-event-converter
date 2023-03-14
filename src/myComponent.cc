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
    if (isPortConnected("rtr_0")) {
        link = configureLink("rtr_0", "50 ps",
                             new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
        rtr_0_Link = link;
    } else {
        out.fatal(CALL_INFO, -1, "%s, Error: rtr_0_Link has no connection'\n", getName().c_str());
    }
    if (isPortConnected("memory")) {
        link = configureLink("memory", "50 ps",
                             new SST::Event::Handler<MyComponent>(this, &MyComponent::handleMemoryEvent));
        memoryLink = link;
    } else {
        out.fatal(CALL_INFO, -1, "%s, Error: memoryLink has no connection'\n", getName().c_str());
    }
}

void MyComponent::init(unsigned int phase) {
    if (!rtr_0_Link)
        out.fatal(CALL_INFO, -1, "%s, Error: unable to configure link on rtr_0_Link\n", getName().c_str());

    if (!memoryLink)
        out.fatal(CALL_INFO, -1, "%s, Error: unable to configure link memoryLink'\n", getName().c_str());

    SST::Event *ev;
    while ((ev = rtr_0_Link->recvInitData())) {
        SST::MemHierarchy::MemEventInit* memEvent = dynamic_cast<SST::MemHierarchy::MemEventInit*>(ev);
        if (memEvent) {
            memoryLink->sendInitData(memEvent->clone());
        }
        delete memEvent;
    }

    while ((ev = memoryLink->recvInitData())) {
        SST::MemHierarchy::MemEventInit* memEvent = dynamic_cast<SST::MemHierarchy::MemEventInit*>(ev);
        delete memEvent;
    }

}

void MyComponent::handleNetworkEvent(SST::Event* ev) {
    // Extract the encapsulated MemEvent
    //out.fatal(CALL_INFO, -1, "%s, received something on port rtr_0\n", getName().c_str());
    SST::MemHierarchy::MemEventBase* memEvent = static_cast<SST::MemHierarchy::MemEventBase*>(ev);
    memoryLink->send(memEvent);
}

void MyComponent::handleMemoryEvent(SST::Event *ev) {
    //out.fatal(CALL_INFO, -1, "%s, received somethign on port memory\n", getName().c_str());
    SST::MemHierarchy::MemEventBase* memEvent = static_cast<SST::MemHierarchy::MemEventBase*>(ev);
    rtr_0_Link->send(memEvent);
}
