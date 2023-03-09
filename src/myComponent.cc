#include <sstream>

#include <sst/core/sst_config.h>
#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/componentInfo.h>
#include <sst/core/sst_types.h>
#include "sst/core/interfaces/stdMem.h"

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

    link = configureLink("memory", "50 ps", new SST::Event::Handler<MyComponent>(this, &MyComponent::handleMemoryEvent));
    memoryLink = link;
}

void MyComponent::init(unsigned int phase) {

}

void MyComponent::handleNetworkEvent(SST::Event* ev) {
    // Extract the encapsulated MemEvent
    SST::Merlin::RtrEvent* rtrEvent = static_cast<SST::Merlin::RtrEvent*>(ev);
    SST::Interfaces::SimpleNetwork::Request* rtrReq = static_cast<SST::Interfaces::SimpleNetwork::Request*>(rtrEvent->takeRequest());
    SST::MemHierarchy::MemEventBase* memEvent = static_cast<SST::MemHierarchy::MemEventBase*>(rtrReq->takePayload());
    delete rtrEvent;
    delete rtrReq;

    // Send the event through the memory port
    memoryLink->send(memEvent);
}

void MyComponent::handleMemoryEvent(SST::Event *ev) {
    /*
    SST::MemHierarchy::MemEventBase* memEvent = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);

// Get the name of the sending component
    std::string sendingComponentName = memEvent->getSrc();
// Use the name to get the sending component's info
    SST::ComponentInfo* sendingComponentInfo = SST::Simulation::getComponent(sendingComponentName);
// Get the sending component's nid_t
    SST::nid_t sendingComponentId = sendingComponentInfo->getId();

    // create a SimpleNetwork::Request with the MemEventBase as payload
    SST::Interfaces::SimpleNetwork::Request *req = new SST::Interfaces::SimpleNetwork::Request();
    req->dest = sendingComponentId; // set destination address
    req->src = sendingComponentId; // set source address
    req->vn = 0; // set virtual network number
    req->size = event->getPayloadSize(); // set payload size
    req->givePayload(event); // set MemEventBase as payload

    // Create a new Merlin::RtrEvent event
    SST::Merlin::RtrEvent *rtrEvent = new SST::Merlin::RtrEvent(req, src, 0);

    // Set the contents of the rtrEvent
    rtrEvent->setSrc(src);
    rtrEvent->setDest(dest);
    rtrEvent->setPayload(memEvent);
    delete memEvent;
    // Send the Merlin::RtrEvent to the desired destination component
    networkLink->send(rtrEvent);
     */

    SST::MemHierarchy::MemEventBase* memEvent = dynamic_cast<SST::MemHierarchy::MemEventBase*>(ev);
    if (memEvent) {
        // Event is a memEventBase event

    } else {
        // Event is not a memEventBase event
        SST::Interfaces::StandardMem::Request* memRequest = dynamic_cast<SST::Interfaces::StandardMem::Request*>(ev);
        if (memRequest){
            // Event is actually a StandardMemory::Request

        } else {
            // Event is not a memEventBase nor a StandardMemory::Request

        }
        delete memRequest;
    }
    delete memEvent;
}
