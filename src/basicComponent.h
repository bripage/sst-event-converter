//
// _basicComponent_h_
//

#ifndef _BASIC_COMPONENT_H_
#define _BASIC_COMPONENT_H_

#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <vector>

namespace SST {
namespace basicComponent {

// --------------------------------------
// basicMsgEvent Event handler
// --------------------------------------
class basicMsgEvent : public SST::Event {
public:
  // Basic Constructor : inherits SST::Event()
  basicMsgEvent() : SST::Event() { }

  // Overloaded Constructor
  basicMsgEvent(std::vector<int> Msg) : SST::Event(), payload(Msg) { }

  // Events must provide a serialization function that serializes
  // all data members of the event
  void serialize_order(SST::Core::Serialization::serializer &ser)  override {
    Event::serialize_order(ser);
    ser & payload;
  }

  std::vector<int> getPayload() { return payload; }

  // Register this event as serializable
  ImplementSerializable(SST::basicComponent::basicMsgEvent);

private:
  // Message payload
  std::vector<int> payload;
};

// --------------------------------------
// basicSubcomponent Subcomponent
// --------------------------------------
class basicSubcomponent : public SST::SubComponent {
public:
  // Register the subcomponent ELI information
  SST_ELI_REGISTER_SUBCOMPONENT_API(SST::basicComponent::basicSubcomponent)

  // Constructor: Components receive a unique ID and the set of parameters
  //              that were assigned in the simulation configuration script
  basicSubcomponent(SST::ComponentId_t id, SST::Params& params)
    : SubComponent(id) { }

  // Destructor
  virtual ~basicSubcomponent() { }

  // Virtual method to inject send the subcomponent and integer
  virtual void send(int number) = 0;
};

// --------------------------------------
// basicMsg Inherited Subcomponent
// --------------------------------------
class basicMsg : public basicSubcomponent {
public:
  SST_ELI_REGISTER_SUBCOMPONENT_DERIVED(
    basicMsg,
    "basicComponent",
    "basicMsg",
    SST_ELI_ELEMENT_VERSION(1,0,0),
    "basicMsg : simple message handler subcomponent",
    SST::basicComponent::basicSubcomponent
  )

  SST_ELI_DOCUMENT_PARAMS(
    { "clockFreq",  "Frequency of period (with units) of the clock", "1GHz" },
    { "count", "Wait for 'count' integers before flushing a message", "5"}
  )

  // { “name”, “description”, vector of supported events }
  SST_ELI_DOCUMENT_PORTS(
    {"msgPort",
     "Link to another component. Uses an event handler to capture events.",
     { "basicComponent.basicMsgEvent", ""}}
  )

  // Constructor
  basicMsg(ComponentId_t id, Params& params);

  // Virtual Destructor
  virtual ~basicMsg();

  // Virtual send function
  virtual void send(int number);

  // Virtual clock handler
  virtual bool clock(Cycle_t cycle);

private:
  // Event handler
  void handleEvent(SST::Event *ev);

  // Params
  SST::Output* out;           // SST Output object for printing, messaging, etc
  std::string clockFreq;      // Clock frequency
  unsigned count;             // Number of integers to store before flushing a message
  std::vector<int> msgs;      // Message storage

  SST::Link *linkHandler;     // Link handler object
};

// --------------------------------------
// basicClock Component
// --------------------------------------
class basicClock : public SST::Component
{
public:

  // Register the component with the SST element library
  SST_ELI_REGISTER_COMPONENT(
    basicClock,                             // Component class
    "basicComponent",                       // Component library
    "basicClock",                           // Component name
    SST_ELI_ELEMENT_VERSION(1,0,0),         // Version of the component
    "basicClock: simple clocked component", // Description of the component
    COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
  )

  // Document the parameters that this component accepts
  // { "parameter_name", "description", "default value or NULL if required" }
  SST_ELI_DOCUMENT_PARAMS(
    { "clockFreq",  "Frequency of period (with units) of the clock", "1GHz" },
    { "clockTicks", "Number of clock ticks to execute",              "500"  }
  )

  // [Optional] Document the ports: we do not define any
  SST_ELI_DOCUMENT_PORTS()

  // Document the statisitcs: we do not define any
  SST_ELI_DOCUMENT_STATISTICS(
    {"EvenCycleCounter", "Counts even numbered cycles", "unitless", 1},
    {"OddCycleCounter",  "Counts odd numbered cycles",  "unitless", 2},
    {"HundredCycleCounter", "Counts every 100th cycle", "unitless", 3}
  )

  // Document the subcomponents
  SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
    {"msg", "Message Interface", "SST::basicComponent::basicMsg"}
  )

  // Class members

  // Constructor: Components receive a unique ID and the set of parameters
  //              that were assigned in the simulation configuration script
  basicClock(SST::ComponentId_t id, SST::Params& params);

  // Destructor
  ~basicClock();

private:

  // Clock handler
  bool clock(SST::Cycle_t cycle);

  // Params
  SST::Output* out;       // SST Output object for printing, messaging, etc
  std::string clockFreq;  // Clock frequency
  Cycle_t cycleCount;     // Cycle counter
  Cycle_t printInterval;  // Interval at which to print to the console

  SST::RNG::MarsagliaRNG* rng;  // Random number generator

  // Subcomponents
  basicSubcomponent *msg; // basicSubcomponent

  // Statistics
  Statistic<uint64_t>* EvenCycles;    // Even cycle statistics counter
  Statistic<uint64_t>* OddCycles;     // Odd cycle statistics counter
  Statistic<uint32_t>* HundredCycles; // Hundred cycle statistics counter

};  // class basicClocks
}   // namespace basicComponent
}   // namespace SST

#endif

