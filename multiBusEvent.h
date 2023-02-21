#ifndef _H_SST_MEM_H_REQUEST_GEN_EVENT
#define _H_SST_MEM_H_REQUEST_GEN_EVENT

#include <stdint.h>
#include <sst/core/event.h>
#include <sst/core/params.h>

namespace SST {
    namespace MultiBus {

        class MultiBusReqEvent : public SST::Event {
        public:
            struct Generator {
                std::string name;
                SST::Params params;
            };

            std::deque< std::pair< std::string, SST::Params> > generators;

            uint64_t 	key;
        private:

            void serialize_order(SST::Core::Serialization::serializer &ser)  override {
                Event::serialize_order(ser);
                ser & key;
                ser & generators;
            }

            ImplementSerializable(SST::MultiBus::MultiBusReqEvent);
        };

        class MultiBusRspEvent : public SST::Event {
        public:
            uint64_t 	key;
        private:
            void serialize_order(SST::Core::Serialization::serializer &ser)  override {
                Event::serialize_order(ser);
                ser & key;
            }
            ImplementSerializable(SST::MultiBus::MultiBusRspEvent);
        };


    }
}

#endif