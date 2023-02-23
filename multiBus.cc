#include <sst/core/sst_config.h>
#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/event.h>
#include <sstream>
#include <iostream>
#include "multiBus.h"

using namespace std;
using namespace SST::MultiBus;

const SST::Event::id_type ANY_KEY = pair<uint64_t, int>((uint64_t)-1, -1);

MultiBus::MultiBus(SST::ComponentId_t id, SST::Params& params) : Component(id) {
    configureParameters(params);
    configureLinks();
    idleCount_ = 0;
    busOn_ = true;
}

void MultiBus::processIncomingEvent(SST::Event* ev) {
    eventQueue_.push(ev);
    if (!busOn_) {
        reregisterClock(defaultTimeBase_, clockHandler_);
        busOn_ = true;
        idleCount_ = 0;
    }
}

bool MultiBus::clockTick(Cycle_t time) {
    if (eventQueue_.empty())
        idleCount_++;

    if (idleCount_ > idleMax_) {
        busOn_ = false;
        idleCount_ = 0;
        return true;
    }

    while (!eventQueue_.empty()) {
        SST::Event* event = eventQueue_.front();
        broadcastEvent(event);
        eventQueue_.pop();
        idleCount_ = 0;

        if (drain_ == 0 )
            break;
    }

    return false;
}

void MultiBus::broadcastEvent(SST::Event* ev) {
    //SST::Link* srcLink = lookupNode(ev->getLastPort());

    for (int i = 0; i < numPorts_; i++) {
        //if (ports_[i] == srcLink) continue;
        SST::Event* newEV = ev;
        ports_[i]->send(newEV);
    }

}

/*----------------------------------------
 * Helper functions
 *---------------------------------------*/
void MultiBus::mapNodeEntry(const std::string& name, SST::Link* link) {
    std::map<std::string, SST::Link*>::iterator it = nameMap_.find(name);
    if (it != nameMap_.end() ) {
        if (it->second != link)
            dbg_.fatal(CALL_INFO, -1, "%s, Error: MultiBus attempting to map node that has already been mapped\n", getName().c_str());
        return;
    }
    nameMap_[name] = link;
}

SST::Link* MultiBus::lookupNode(const std::string& name) {
    std::map<std::string, SST::Link*>::iterator it = nameMap_.find(name);
    if (nameMap_.end() == it) {
        dbg_.fatal(CALL_INFO, -1, "%s, Error: MultiBus lookup of node %s returned no mapping\n", getName().c_str(), name.c_str());
    }
    return it->second;
}

void MultiBus::configureLinks() {
    std::cout << "Begining configureLinks()" << std::endl;

    SST::Link* link;
    std::string linkprefix = "port";
    std::string linkname = linkprefix + "0";
    while (isPortConnected(linkname)) {
        link = configureLink(linkname, "50 ps", new Event::Handler<MultiBus>(this, &MultiBus::processIncomingEvent));
        if (!link)
            dbg_.fatal(CALL_INFO, -1, "%s, Error: unable to configure link on port '%s'\n", getName().c_str(), linkname.c_str());
        ports_.push_back(link);
        numPorts_++;
        linkname = linkprefix + std::to_string(numPorts_);
    }

    if (numPorts_ < 1) dbg_.fatal(CALL_INFO, -1,"couldn't find number of Ports (numPorts)\n");
    std::cout << "ConfigLinks() done" << std::endl;
}

void MultiBus::configureParameters(SST::Params& params) {
    numPorts_  = 0;
    latency_      = params.find<uint64_t>("bus_latency_cycles", 1);
    idleMax_      = params.find<uint64_t>("idle_max", 6);
    busFrequency_ = params.find<std::string>("bus_frequency", "Invalid");
    drain_        = params.find<bool>("drain_bus", 0);

    if (busFrequency_ == "Invalid") dbg_.fatal(CALL_INFO, -1, "Bus Frequency was not specified\n");

    /* Multiply Frequency times two.  This is because an SST MultiBus components has
       2 SST Links (highNEt & LowNet) and thus it takes a least 2 cycles for any
       transaction (a real bus should be allowed to have 1 cycle latency).  To overcome
       this we clock the bus 2x the speed of the cores */

    UnitAlgebra uA = UnitAlgebra(busFrequency_);
    uA = uA * 2;
    busFrequency_ = uA.toString();

    clockHandler_ = new Clock::Handler<MultiBus>(this, &MultiBus::clockTick);
    defaultTimeBase_ = registerClock(busFrequency_, clockHandler_);
}
