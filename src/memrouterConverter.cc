#include "memrouterConverter.h"

using namespace SST;

// Constructor
MemRouterConverter::MemRouterConverter(SST::ComponentId_t id, SST::Params& params) :
        Component(id)
{
    // Get parameters
    output.init("", 0, 0, SST::Output::STDOUT);

    network_num_vc = params.find<int>("network_num_vc", 1);

    // Set up SimpleNetwork port
    std::string network_port_name = params.find<std::string>("network_port_name", "network");
    simpleNetwork = dynamic_cast<SST::Interfaces::SimpleNetwork*>(
            this->providePort(network_port_name, new SST::Interfaces::SimpleNetwork::Handler<MemRouterConverter>(this, &MemRouterConverter::handleNetworkEvent))
    );

    // Set up StandardMemory port
    std::string memory_port_name = params.find<std::string>("memory_port_name", "memory");
    standardMemory = dynamic_cast<SST::Interfaces::StandardMem*>(
            this->providePort(memory_port_name, new SST::Interfaces::StandardMem::Handler<MemRouterConverter>(this, &MemRouterConverter::handleMemoryEvent))
    );
}

SST::Port* MemRouterConverter::providePort(const std::string& portName, int portIndex, SST::Port::PortFlags flags) {
    // Check if the port is a memory port
    if (portName == "memory") {
        // Check if the port index is valid
        if (portIndex != 0) {
            // Invalid port index
            return nullptr;
        }
        // Return a pointer to the memory port
        return memoryPort;
    }
    // Check if the port is a network port
    if (portName == "network") {
        // Check if the port index is valid
        if (portIndex != 0) {
            // Invalid port index
            return nullptr;
        }
        // Return a pointer to the network port
        return networkPort;
    }
    // Invalid port name
    return nullptr;
}


// Handle incoming events on the SimpleNetwork port
void MemRouterConverter::handleNetworkEvent(SST::Interfaces::SimpleNetwork::Request* event)
{
    // Convert SimpleNetwork event to StandardMemory event
    SST::Interfaces::StandardMem::Request* memReq = convertToStandardMemory(event);

    // Send StandardMemory event to memory
    standardMemory->send(memReq);
}

// Handle incoming events on the StandardMemory port
void MemRouterConverter::handleMemoryEvent(SST::Interfaces::StandardMem::Request* event)
{
    // Convert StandardMemory event to SimpleNetwork event
    SST::Interfaces::SimpleNetwork::Request* netReq = convertToSimpleNetwork(event);

    // Send SimpleNetwork event to network
    simpleNetwork->send(netReq);
}

// Convert SimpleNetwork event to StandardMemory event
SST::Interfaces::StandardMem::Request* MemRouterConverter::convertToStandardMemory(SST::Interfaces::SimpleNetwork::Request* netReq)
{
    SST::Interfaces::StandardMem::Request* memReq = new SST::Interfaces::StandardMem::Request();

    memReq->addr = netReq->dest;
    memReq->size = netReq->size;
    memReq->cmd = SST::Interfaces::StandardMem::Command::Read;

    return memReq;
}

// Convert StandardMemory event to SimpleNetwork event
SST::Interfaces::SimpleNetwork::Request* MemRouterConverter::convertToSimpleNetwork(SST::Interfaces::StandardMem::Request* memReq)
{
    SST::Interfaces::SimpleNetwork::Request* netReq = new SST::Interfaces::SimpleNetwork::Request();

    netReq->dest = memReq->addr;
    netReq->size = memReq->size;
    netReq->vn = 0;

    return netReq;
}


// Document
