#include <sst_config.h>

#include <sstream>

#include "multiBus.h"

#include <sst/core/params.h>
#include <sst/core/interfaces/stringEvent.h>
#include "memEvent.h"
#include "memEventBase.h"

using namespace std;
using namespace SST;
using namespace SST::MemHierarchy;


const MultiBus::key_t MultiBus::ANY_KEY = std::pair<uint64_t, int>((uint64_t)-1, -1);

MultiBus::MultiBus(ComponentId_t id, Params& params) : Component(id) {
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

        if (broadcast_)
            broadcastEvent(event);
        else
            sendSingleEvent(event);

        eventQueue_.pop();
        idleCount_ = 0;

        if (drain_ == 0 )
            break;
    }

    return false;
}


void MultiBus::broadcastEvent(SST::Event* ev) {
    MemEventBase* memEvent = static_cast<MemEventBase*>(ev);
    SST::Link* srcLink = lookupNode(memEvent->getSrc());

    for (int i = 0; i < numHPorts_; i++) {
        if (ports_[i] == srcLink) continue;
        ports_[i]->send(memEvent->clone());
    }

    delete memEvent;
}



void MultiBus::sendSingleEvent(SST::Event* ev) {
    MemEventBase *event = static_cast<MemEventBase*>(ev);
#ifdef __SST_DEBUG_OUTPUT__
    if (is_debug_event(event)) {
        dbg_.debug(_L3_, "E: %-20" PRIu64 " %-20" PRId32 " %-20s Event:New     (%s)\n",
                getCurrentSimCycle(), 0, getName().c_str(), event->getVerboseString().c_str());
        fflush(stdout);
    }
#endif
    SST::Link* dstLink = lookupNode(event->getDst());
    MemEventBase* forwardEvent = event->clone();
    dstLink->send(forwardEvent);

    delete event;
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

}

void MultiBus::configureParameters(SST::Params& params) {
    int debugLevel = params.find<int>("debug_level", 0);

    dbg_.init("", debugLevel, 0, (Output::output_location_t)params.find<int>("debug", 0));
    if (debugLevel < 0 || debugLevel > 10)     dbg_.fatal(CALL_INFO, -1, "Debugging level must be between 0 and 10. \n");

    std::vector<Addr> addrArr;
    params.find_array<Addr>("debug_addr", addrArr);
    for (std::vector<Addr>::iterator it = addrArr.begin(); it != addrArr.end(); it++)
        DEBUG_ADDR.insert(*it);

    numPorts_  = 0;

    latency_      = params.find<uint64_t>("bus_latency_cycles", 1);
    idleMax_      = params.find<uint64_t>("idle_max", 6);
    busFrequency_ = params.find<std::string>("bus_frequency", "Invalid");
    broadcast_    = params.find<bool>("broadcast", 1);
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

void MultiBus::init(unsigned int phase) {
    SST::Event *ev;

    for (int i = 0; i < numPorts_; i++) {
        while ((ev = ports_[i]->recvInitData())) {
            MemEventInit* memEvent = dynamic_cast<MemEventInit*>(ev);

            if (memEvent && memEvent->getCmd() == Command::NULLCMD) {
                dbg_.debug(_L10_, "multiBus %s broadcasting event to all other ports (%d): %s\n", getName().c_str(), numPorts_, memEvent->getVerboseString().c_str());
                mapNodeEntry(memEvent->getSrc(), ports_[i]);
                for (int k = 0; k < numPorts_; k++)
                    if (ports_[k] == srcLink) continue;
                    ports_[k]->sendInitData(memEvent->clone());
            } else if (memEvent) {
                dbg_.debug(_L10_, "multiBus %s broadcasting event to all other ports (%d): %s\n", getName().c_str(), numPorts_, memEvent->getVerboseString().c_str());
                for (int k = 0; k < numPorts_; k++)
                    if (ports_[k] == srcLink) continue;
                    ports_[k]->sendInitData(memEvent->clone());
            }
            delete memEvent;
        }
    }
}