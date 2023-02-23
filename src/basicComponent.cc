
//
// _basicComponent_cc_
//

#include <sst/core/sst_config.h>
#include "basicComponent.h"

using namespace SST;
using namespace SST::basicComponent;

// basicMsg constructor
basicMsg::basicMsg(ComponentId_t id, Params& params)
  : basicSubcomponent(id,params){

  // Create a new SST output object
  out = new Output("", 1, 0, Output::STDOUT);

  // Read the parameters
  count = params.find<unsigned>("count",5);
  clockFreq  = params.find<std::string>("clockFreq", "1GHz");

  // Register the clock handler
  registerClock(clockFreq, new Clock::Handler<basicMsg>(this, &basicMsg::clock));
  out->output("Registering subcomponent clock with frequency=%s\n", clockFreq.c_str());

  // Configure the links
  linkHandler = configureLink("msgPort", new Event::Handler<basicMsg>(this, &basicMsg::handleEvent));
}

// basicMsg destructor
basicMsg::~basicMsg(){
  delete out;
}

// basicMsg send function
void basicMsg::send(int number){
  msgs.push_back(number);
}

// basicMsg event handler
void basicMsg::handleEvent(SST::Event *ev){
  basicMsgEvent *event = dynamic_cast<basicMsgEvent*>(ev);

  if( event ){
    std::vector<int> msgs = event->getPayload();

    if( msgs.size() > 0 ){
      out->output("Received message buffer at clock%" PRIu64 "\n", getCurrentSimCycle());
      for( unsigned i=0; i<count; i++ ){
        out->output("Received buffer[%d]: %d\n", i, msgs[i]);
      }
    }
    delete event;
  }else{
    out->fatal(CALL_INFO, -1, "Error: Bad event type received!\n" );
  }
}

// basicMsg clock handler
bool basicMsg::clock(Cycle_t cycle){
  if( msgs.size() >= count ){
    out->output("Flushing message buffer at clock=%" PRIu64 "\n", cycle );
    for( unsigned i=0; i<count; i++ ){
      out->output("Sent Buffer[%d]: %d\n", i, msgs[i]);
    }

    basicMsgEvent *event = new basicMsgEvent(msgs);
    linkHandler->send(event);

    msgs.clear();
  }
  return false;
}

// basicClock constructor
// - Read the parameters
// - Register the clock handler
basicClock::basicClock(ComponentId_t id, Params& params)
  : Component(id) {

  // Create a new SST output object
  out = new Output("", 1, 0, Output::STDOUT);

  // Retrieve the paramaters from the simulation script
  clockFreq  = params.find<std::string>("clockFreq", "1GHz");
  cycleCount = params.find<Cycle_t>("clockTicks", "500");

  // Tell the simulation not to end until we signal completion
  registerAsPrimaryComponent();
  primaryComponentDoNotEndSim();

  // Register the clock handler
  registerClock(clockFreq, new Clock::Handler<basicClock>(this, &basicClock::clock));

  out->output("Registering clock with frequency=%s\n", clockFreq.c_str());

  // Register statistics
  EvenCycles = registerStatistic<uint64_t>("EvenCycleCounter");
  OddCycles  = registerStatistic<uint64_t>("OddCycleCounter");
  HundredCycles = registerStatistic<uint32_t>("HundredCycleCounter");

  // Load the subcomponent
  msg = loadUserSubComponent<basicSubcomponent>("msg");

  // Initialize the RNG
  rng = new SST::RNG::MarsagliaRNG(11, 272727);

  // This component prints the clock cycles & time every so often so calculate a print interval
  // based on simulation time
  printInterval = cycleCount / 10;
  if (printInterval < 1)
    printInterval = 1;
}

// basicClock destructor
basicClock::~basicClock(){
  delete out;
  delete rng;
}

// main clock handler
bool basicClock::clock(Cycle_t cycles){

  // Print time in terms of clock cycles every `printInterval` clocks
  if( (cycles % printInterval) == 0 ){
    out->output("Clock cycles: %" PRIu64 ", SimulationCycles: %" PRIu64 ", Simulation ns: %" PRIu64 "\n",
                cycles, getCurrentSimCycle(), getCurrentSimTimeNano());
  }

  // Increment any potential statistics
  if( (cycles % 2) == 0 ){
    // even numbered cycle
    EvenCycles->addData(1);
  }else{
    // odd numbered cycle
    OddCycles->addData(1);
  }

  if( (cycles % 100) == 0 ){
    // every 100th cycle
    HundredCycles->addData(1);
  }

  cycleCount--;

  // inject a message into the subcomponent
  msg->send( rng->generateNextInt32() );

  // If the simulation exit condition has been met,
  // end the simulation
  if( cycleCount == 0 ){
    primaryComponentOKToEndSim();
    return true;
  }else{
    return false;
  }
}
