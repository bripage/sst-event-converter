#include <sstream>

#include <sst/core/sst_config.h>
#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/componentInfo.h>

#include "sst/elements/memHierarchy/memEvent.h"
#include "sst/elements/memHierarchy/memEventBase.h"
#include "sst/elements/merlin/router.h"
#include "myComponent.h"


MyComponent::MyComponent(SST::ComponentId_t id, SST::Params &params) : Component(id) {
    configureLinks();
}

void MyComponent::configureLinks() {
    SST::Link* link;
    link = configureLink("network", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));

    networkLink = link;
    link = configureLink("memory", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleNetworkEvent));
    memoryLink = link;
}

void MyComponent::init(unsigned int phase) {

}

void MyComponent::handleNetworkEvent(SST::Event* ev) {
    // Extract the encapsulated MemEvent
    SST::Merlin::RtrEvent* rtrEvent = static_cast<SST::Merlin::RtrEvent*>(ev);
    SST::MemHierarchy::MemEventBase* memEvent = static_cast<SST::MemHierarchy::MemEventBase*>(rtrEvent->getPayload());
    delete rtrEvent;

    // Send the event through the memory port
    memoryLink->send(memEvent);
}

void MyComponent::handleMemoryEvent(SST::Event *ev) {
    SST::MemHierarchy::MemEventBase* memEvent = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    SST::ComponentInfo* comp_info = getComponentInfo();

    // Retrieve the destination component from the memory event
    std::string src_name = memEvent->getSrc();
    std::string dest_name = memEvent->getDst();
    SST::ComponentId_t comp_id = comp_info->findComponent(src_name);
    SST::ComponentId_t dest = comp_id;
    comp_id = comp_info->findComponent(dest_name);
    SST::ComponentId_t src = comp_id;

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
