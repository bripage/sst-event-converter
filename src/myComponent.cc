#include "myComponent.h"

using namespace SST;
using namespace SST::Interfaces;
using namespace SST::MemHierarchy;
using namespace SST::Merlin;

myComponent::myComponent(ComponentId_t id, Params& params) :
        Component(id) {

    // get the parameters
    mem_link_name = params.find<std::string>("mem_link", "memory");
    net_link_name = params.find<std::string>("net_link", "network");
    network_num_vcs = params.find<int>("network_num_vc", 1);
    output = new Output("myComponent[@p:@l]: ", params.find<int>("verbose", 0), 0, Output::STDOUT);

    // set up the ports
    // setup the memory port
    port_memory = dynamic_cast<SimpleMem*>(loadModuleWithComponent(mem_link_name, this));
    if (!port_memory) {
        output->fatal(CALL_INFO, -1, "Error loading memory component %s\n", mem_link_name.c_str());
    }

    // set up the network port
    int64_t const def_mem_size = 512*1024*1024; // default memory size of 512MB
    int64_t max_addr = params.find<int64_t>("max_addr", def_mem_size - 1);
    max_addr = max_addr < (def_mem_size - 1) ? max_addr : def_mem_size - 1;

    Params router_params = params.find_prefix_params("network.");

    port_network = configureLink(net_link_name, new Event::Handler<myComponent>(this, &myComponent::handleNetworkEvent));
    if (!port_network) {
        output->fatal(CALL_INFO, -1, "Failed to configure network link with name %s\n", net_link_name.c_str());
    }

    output->verbose(CALL_INFO, 1, 0, "Configured with memory link: %s, network link: %s, max addr: %" PRIu64 "\n", mem_link_name.c_str(), net_link_name.c_str(), max_addr);
}

void myComponent::init(unsigned int phase) {
    output->verbose(CALL_INFO, 1, 0, "Initialized myComponent\n");
}

bool myComponent::tick(SST::Cycle_t cycle) {
    output->verbose(CALL_INFO, 1, 0, "Ticking myComponent\n");
    return false;
}

void myComponent::setup() {
    output->verbose(CALL_INFO, 1, 0, "Setting up myComponent\n");
}

void myComponent::finish() {
    output->verbose(CALL_INFO, 1, 0, "Finishing myComponent\n");
}

bool MyComponent::handleNetworkEvent(int port, SST::Interfaces::SimpleNetwork::Request* req)
{
    // Check if the event is of type Merlin::Event
    if (req->inspectPayloadType<Merlin::Event>()) {
        // Extract the encapsulated MemEvent
        Merlin::Event* ev = static_cast<Merlin::Event*>(req->takePayload());
        MemEvent* memEv = static_cast<MemEvent*>(ev->getPayload());
        delete ev;

        // Convert the MemEvent to a StandardMemory event
        SST::Interfaces::StandardMemory::MemEvent* stdMemEv = convertToStandardMemory(memEv);

        // Send the event through the memory port
        memPort->sendEvent(stdMemEv);

        return true;
    }

    // The event is not of the expected type
    return false;
}

void MyComponent::handleMemoryEvent(SST::Event *ev, int id)
{
    MemEvent *mem_event = dynamic_cast<MemEvent*>(ev);
    if (mem_event) {
        std::cout << "Received a memory event with address: " << mem_event->getAddr() << std::endl;

        SimpleNetwork::Request *network_req = new SimpleNetwork::Request();
        network_req->dest = m_network_address;
        network_req->src = m_component_id;
        network_req->vn = 0;
        network_req->type = SimpleNetwork::Request::WRITE;
        network_req->givePayload(new SimpleMemEvent(mem_event));

        m_link->send(network_req);
    } else {
        std::cerr << "Received an unknown event type in handleMemoryEvent" << std::endl;
    }
}

SST::Interfaces::StandardMemory::Request* MyComponent::convertToStandardMemory(MemEvent* event) {
    SST::Interfaces::StandardMemory::Request *req = new SST::Interfaces::StandardMemory::Request();
    req->addr = event->getAddr();
    req->size = event->getSize();
    req->flags = 0;
    req->id = event->getID();
    req->getData().insert(req->getData().begin(), event->getPayload().begin(), event->getPayload().end());

    return req;
}
