

namespace SST {
    namespace MemHierarchy {

        using namespace std;

        class ConversionEvent : public SST::Event {
        public:

            /** Creates a new MemEventBase */
            ConversionEvent(SST::Event, std::string src) : SST::Event() {
                if (src == "router"){
                    /** convert router event to a memory event */


                } else {
                    /** convert memory event to a router event */


                }
            }

        private:

        };

    }
}