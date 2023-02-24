#ifndef _EVENTCONVERTER_H
#define _EVENTCONVERTER_H

#include <queue>
#include <map>
#include <sst/core/event.h>
#include <sst/core/sst_types.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/timeConverter.h>
#include <sst/core/output.h>
#include <sst/core/eli/elementinfo.h>

using namespace std;

namespace SST { 
    namespace EventConverter {


        class BusEvent;

/*
 *  Component: eventConverter
 *
 *  Connects two or more components over a bus like interface. The component types need not be the same type and may
 *  include multiple component types and or parameter settings. All messages are broadcast to all connected components.
 */

        class EventConverter : public SST::Component {
        public:

            // REGISTER THIS COMPONENT INTO THE ELEMENT LIBRARY
            SST_ELI_REGISTER_COMPONENT(
                    EventConverter,
            "eventConverter",
            "EventConverter",
            SST_ELI_ELEMENT_VERSION(1,0,0),
            "EventConverter serves as an intermediary between router and memory components. It converts memHierachy memory events (inheriting from STD::Interfaces::StandardMem) to those of router components (STD::Interfaces::SimpleNetwork) as well as the reverse. Events received on router ports are assumed to be destined for memory components attached to the memory ports and the event contents are converted to match the appropriate contents and serialized ordering. The opposite is true for messages received on a memory port. ",
            COMPONENT_CATEGORY_PROCESSOR
            )

            EventConverter(SST::ComponentId_t id, SST::Params& params);
            virtual void init(unsigned int phase);
            typedef SST::Event::id_type key_t;
            static const key_t ANY_KEY;
            static const char BUS_INFO_STR[];

            SST_ELI_DOCUMENT_PARAMS(
            {"conversion_latency",  "(uint) event conversion latency in cycles", "0"},)

            SST_ELI_DOCUMENT_PORTS(
            {"router", "Ports connecting to a router component (merlin, kingsley, etc.)", "SST::Interfaces::SimpleNetwork" },
            {"memory", "Ports connecting to a memory component (memHiearchy, etc.)", "SST::Interfaces::StandardMem" } )

        /* Class definition */
        private:

            /** Adds event to the incoming event queue.  Reregisters clock if needed */
            void EventConverter::processIncomingRouterEvent(Event* ev);
            void EventConverter::processIncomingMemoryEvent(Event* ev);

            void EventConverter::routerToMemoryConversion(Event* ev);
            void EventConverter::memoryToRouterConversion(Event* ev);
            /**  Clock Handler */
            bool clockTick(Cycle_t);

            /** Configure Bus objects with the appropriate parameters */
            void configureParameters(SST::Params&);
            void configureLinks();

            void mapNodeEntry(const std::string&, SST::Link*);
            SST::Link* lookupNode(const std::string&);

            Output                          dbg_;
            int                             numPorts_;
            uint64_t                        conversionLatency_;
            Clock::Handler<EventConverter>*       clockHandler_;
            TimeConverter*                  defaultTimeBase_;
            std::map<string,SST::Link*>     nameMap_;
            SST::Link*                      router_;
            SST::Link*                      memory_;
            std::queue<SST::Event*>         routerEventQueue_;
            std::queue<SST::Event*>         memoryEventQueue_;

        };

    }}
#endif /* SST_EventConverter_H */

