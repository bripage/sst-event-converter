#ifndef SST_MULTIBUS_H
#define SST_MULTIBUS_H
/*
 *  Component: multiBus
 *
 *  Connects two or more components over a bus like interface. The component types need not be the same type and may
 *  include multiple component types and or parameter settings. All messages are broadcast to all connected components.
 */
#include <sst/core/component.h>

using namespace SST;
using namespace SST::Interfaces;

namespace SST {
    namespace MultiBus {

        class MultiBus : public SST::Component {
        public:
            // Class members

            // Constructor: Components receive a unique ID and the set of parameters
            //              that were assigned in the simulation configuration script
            MultiBus(SST::ComponentId_t id, SST::Params& params);

            // Destructor
            ~MultiBus();

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
            // Params
            TimeConverter* timeConverter;
            Clock::HandlerBase* clockHandler;
            std::string busFrequency;
            std::string bus_latency_cycles;
            std::vector<SST::Link *> ports_;
            std::map<string, SST::Link *> nameMap_;
            std::queue<SST::Event *> eventQueue_;
            Output* out;

            void handleEvent( StandardMem::Request* ev );
            bool clockTick( SST::Cycle_t );
            void handleSrcEvent( SST::Event* );
        };

    }
}

#endif /* SST_MULTIBUS_H */

