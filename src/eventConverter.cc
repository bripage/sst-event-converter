#include <sst/core/sst_config.h>
#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/event.h>
#include <sstream>
#include <sst/core/timeConverter.h>

#include "eventConverter.h"
#include "conversionEvent.h"
using namespace std;
using namespace SST::EventConverter;

const SST::Event::id_type ANY_KEY = pair<uint64_t, int>((uint64_t)-1, -1);

EventConverter::EventConverter(SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
    configureParameters(params);
    configureLinks();
    //registerAsPrimaryComponent();
    //primaryComponentDoNotEndSim();
}

// where the work happens per clock cycle
bool EventConverter::clockTick(Cycle_t time) {
    while (!routerEventQueue_.empty()) {
        SST::Event* event = routerEventQueue_.front();
        SST::Link* dstLink = lookupNode(event->getDst());

        RouterEvent* forwardEvent = event->clone();
        dstLink->send(forwardEvent);
        routerEventQueue_.pop();

        delete event;
    }

    while (!memoryEventQueue_.empty()) {
        SST::Event* event = memoryEventQueue_.front();
        SST::Link* dstLink = lookupNode(event->getDst());

        MemEventBase* forwardEvent = event->clone();
        dstLink->send(forwardEvent);
        memoryEventQueue_.pop();

        delete event;
    }
    //primaryComponentOKToEndSim();
    return false;
}

void EventConverter::processIncomingRouterEvent(Event* ev) {
    conversionEvent* event = static_cast<conversionEvent*>(ev, "router");
    routerEventQueue_.push(event);
}

void EventConverter::processIncomingMemoryEvent(Event* ev) {
    conversionEvent* event = static_cast<conversionEvent*>(ev, "memory");
    memoryEventQueue_.push(event);
}

/*----------------------------------------
 * Helper functions
 *---------------------------------------*/
void EventConverter::mapNodeEntry(const std::string& name, SST::Link* link) {
    std::map<std::string, SST::Link*>::iterator it = nameMap_.find(name);
    if (it != nameMap_.end() ) {
        if (it->second != link)
            dbg_.fatal(CALL_INFO, -1, "%s, Error: EventConverter attempting to map node that has already been mapped\n", getName().c_str());
        return;
    }
    nameMap_[name] = link;
}

SST::Link* EventConverter::lookupNode(const std::string& name) {
    std::map<std::string, SST::Link*>::iterator it = nameMap_.find(name);
    if (nameMap_.end() == it) {
        dbg_.fatal(CALL_INFO, -1, "%s, Error: EventConverter lookup of node %s returned no mapping\n", getName().c_str(), name.c_str());
    }
    return it->second;
}

void EventConverter::configureLinks() {
    SST::Link* link;
    std::string rtrlinkprefix = "router";
    std::string rtrlinkname = linkprefix;
    if (isPortConnected(linkname)) {
        link = configureLink(linkname, "50 ps", new Event::Handler<EventConverter>(this, &EventConverter::processIncomingRouterEvent));
        if (!link)
            dbg_.fatal(CALL_INFO, -1, "%s, Error: unable to configure link on port '%s'\n", getName().c_str(), linkname.c_str());
        ports_.push_back(link);
        numPorts_++;
        //linkname = linkprefix + std::to_string(numPorts_);
    }

    std::string memlinkprefix = "memory";
    std::string memlinkname = linkprefix;
    if (isPortConnected(linkname)) {
        link = configureLink(linkname, "50 ps", new Event::Handler<EventConverter>(this, &EventConverter::processIncomingMemoryEvent));
        if (!link)
            dbg_.fatal(CALL_INFO, -1, "%s, Error: unable to configure link on port '%s'\n", getName().c_str(), linkname.c_str());
        ports_.push_back(link);
        numPorts_++;
        //linkname = linkprefix + std::to_string(numPorts_);
    }
}

void EventConverter::configureParameters(Params& params) {
    numPorts_  = 0;
    conversionLatency_  = params.find<uint64_t>("conversion_latency", 1);
    clockHandler_       = new Clock::Handler<EventConverter>(this, &EventConverter::clockTick);
    defaultTimeBase_    = registerClock(busFrequency_, clockHandler_);
}

void EventConverter::init(unsigned int phase) {
    SST::Event *ev;
    while ((ev = router_->recvInitData())) {
         if (ev && ev->getCmd() == Command::NULLCMD) {
             mapNodeEntry(ev->getSrc(), ports_[i]);
             ports_[k]->sendInitData(ev->clone());
         } else if (ev) {
            router_->sendInitData(ev->clone());
        }
    }

    while ((ev = memory_->recvInitData())) {
         if (ev && ev->getCmd() == Command::NULLCMD) {
             mapNodeEntry(ev->getSrc(), ports_[i]);
             ports_[k]->sendInitData(ev->clone());
         } else if (ev) {
             memory_->sendInitData(ev->clone());
        }
    }
}


