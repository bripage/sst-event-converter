#ifndef _MYCOMPONENT_H
#define _MYCOMPONENT_H

#include <sst/core/event.h>
#include <sst/core/sst_types.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/output.h>
#include "sst/elements/memHierarchy/memEvent.h"
#include "sst/elements/memHierarchy/memEventBase.h"
#include "sst/elements/merlin/router.h"


//namespace SST {
//    namespace MyComponent {

        class MyComponent : public SST::Component {
        public:
            SST_ELI_REGISTER_COMPONENT(MyComponent,
                                       "myComponent", // library name
                                       "MyComponent", // component name -> myComponent.MyComponent
                                       SST_ELI_ELEMENT_VERSION(1,0,0),
                                       "A component that converts between memHierarchy.MemEventBase and merlin.RtrEvent events",
                                       COMPONENT_CATEGORY_UNCATEGORIZED
            )

            SST_ELI_DOCUMENT_PARAMS(

            )

            SST_ELI_DOCUMENT_PORTS(
            { "memory", "Memory interface", {}},
            { "network", "Network interface", {}}
            )


            MyComponent(SST::ComponentId_t id, SST::Params &params);

            void init(unsigned int phase);

        private:
            void configureLinks();
            void handleNetworkEvent(SST::Event *ev);
            void handleMemoryEvent(SST::Event *ev);

            SST::Output out;
            SST::Link* memoryLink;
            SST::Link* networkLink;
        };
//    }
//}

#endif /* _MYCOMPONENT_H */