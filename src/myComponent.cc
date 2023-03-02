#include <sst_config.h>
#include <sstream>
#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>

#include "memEvent.h"
#include "memEventBase.h"
#include "merlin.h"
#include "myComponent.h"

using namespace SST;
using namespace MyComponent;

MyComponent::MyComponent(ComponentId_t id, Params& params) : Component(id) {
    configureLinks()
}

void MyComponent::configureLinks() {
    SST::Link* link;
    std::string linkname = "network";
    if (isPortConnected(linkname)) {
        link = configureLink(linkname, "50 ps", new Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
        networkLink = link;
    }

    std::string linkname = "memory";
    if (isPortConnected(linkname)) {
        link = configureLink(linkname, "50 ps", new Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
        memoryLink = link;
    }
}

void MyComponent::init(unsigned int phase) {
    output->verbose(CALL_INFO, 1, 0, "Initialized myComponent\n");
}

void MyComponent::handleNetworkEvent(SST::Event* ev) {
    // Extract the encapsulated MemEvent
    SST::Merlin::RtrEvent* rtrEvent = static_cast<SST::Merlin::Event*>(ev);
    MemEventBase* memEvent = static_cast<MemEventBase*>(rtrEvent->getPayload());
    delete rtrEvent;

    // Send the event through the memory port
    memoryLink->send(memEvent);
}

void MyComponent::handleMemoryEvent(SST::Event *ev) {
    SST::MemHierarchy::MemEventBase *memEvent = dynamic_cast<SST::MemHierarchy::MemEventBase*>(event);

    // Retrieve the destination component from the memory event
    SST::ComponentId_t dest = memEvent->getDst();
    SST::ComponentId_t src = memEvent->getSrc();
    // Create a new Merlin::RtrEvent event
    SST::Merlin::RtrEvent *rtrEvent = new SST::Merlin::RtrEvent();
    // Set the contents of the rtrEvent
    rtrEvent->setSrc(src);
    rtrEvent->setDest(dest);
    rtrEvent->setPayload(memEvent);
    delete memEvent;
    // Send the Merlin::RtrEvent to the desired destination component
    networkLink->send(rtrEvent);

}
