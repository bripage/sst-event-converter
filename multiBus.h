#ifndef SST_MULTIBUS_H
#define SST_MULTIBUS_H

/*
 *  Component: multiBus
 *
 *  Connects two or more components over a bus like interface. The component types need not be the same type and may
 *  include multiple component types and or parameter settings. All messages are broadcast to all connected components.
 */

#include <queue>
#include <map>
#include <sst/core/event.h>
#include <sst/core/sst_types.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/timeConverter.h>
#include <sst/core/output.h>

using namespace std;

namespace SST {
    namespace MultiBus {





        class BusEvent;

        class MultiBus : public SST::Component {
        public:
            MultiBus(SST::ComponentId_t id, SST::Params &params);

            //virtual void init(unsigned int phase);

            typedef SST::Event::id_type key_t;
            static const key_t ANY_KEY;
            static const char BUS_INFO_STR[];

            /* Element Library Info */
            SST_ELI_REGISTER_COMPONENT(MultiBus,
            "multiBus", "MultiBus", SST_ELI_ELEMENT_VERSION(1,0,0), "Multi component type Bus Model", COMPONENT_CATEGORY_MEMORY)

            SST_ELI_DOCUMENT_PARAMS(
            { "bus_frequency", "(string) Bus clock frequency" },
            { "bus_latency_cycles", "(uint) Bus latency in cycles", "0" },
            { "idle_max", "(uint) Bus temporarily turns off clock after this number of idle cycles", "6" },
            { "drain_bus", "(bool) Drain bus on every cycle", "0" })

            SST_ELI_DOCUMENT_PORTS(
            { "port%(port_number)d", "Ports connected to additional components. Can be different component types. ", {}} )

        private:
            /** Adds event to the incoming event queue.  Reregisters clock if needed */
            void processIncomingEvent(SST::Event *ev);

            /** Broadcast event to all ports */
            void broadcastEvent(SST::Event *ev);

            /**  Clock Handler */
            bool clockTick(Cycle_t);

            /** Configure Bus objects with the appropriate parameters */
            void configureParameters(SST::Params &);

            void configureLinks();

            void mapNodeEntry(const std::string &, SST::Link *);

            SST::Link *lookupNode(const std::string &);


            Output dbg_;
            int numPorts_;
            uint64_t idleCount_;
            uint64_t latency_;
            uint64_t idleMax_;
            bool busOn_;
            bool drain_;
            Clock::Handler <MultiBus> *clockHandler_;
            TimeConverter *defaultTimeBase_;

            std::string busFrequency_;
            std::string bus_latency_cycles_;
            std::vector<SST::Link *> ports_;
            std::map<string, SST::Link *> nameMap_;
            std::queue<SST::Event *> eventQueue_;

        };

    }
}
