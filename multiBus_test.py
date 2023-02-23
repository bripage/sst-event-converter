#import sst
#import sys

# Define SST core options
sst.setProgramOption("timebase", "1 ps")

#-- set the "stop-at" option
sst.setProgramOption("stop-at", "10s")


#
#   Begin Core 1
#
# core1 = sst.Component("core1", "miranda.BaseCPU")
# core1.addParams({
#     "verbose" : 1,
#     "clock" : "2GHz",
#     "generator" : "miranda.STREAMBenchGenerator"
# })
# gen = core1.setSubComponent("gen1", "miranda.STREAMBenchGenerator")
# gen.addParams({
#     "generatorParams.verbose" : 1
# })
#
# L1_1 = sst.Component("L1_1", "memHierarchy.Cache")
# L1_1.addParams({
#     "cache_frequency": "2GHz",
#     "cache_size": "32KB",
#     "associativity": 8,
#     "access_latency_cycles": 4,
#     "L1": 1
# })
#
# L2_1 = sst.Component("L2_1", "memHierarchy.Cache")
# L2_1.addParams({
#     "prefetcher": "cassini.StridePrefetcher",
#     "prefetcher.reach": 16,
#     "prefetcher.detect_range" : 1,
#     "cache_frequency": "2GHz",
#     "cache_size": "256KB",
#     "associativity": 8,
#     "access_latency_cycles": 6,
#     "mshr_num_entries" : 16,
#     "mshr_latency_cycles": 2,
#     "memNIC.network_bw": "51.2GB/s"
# })
#
#
# C1toL1 = sst.Link("C1toL1")
# C1toL1.connect( (core1, "cache_link", "300ps"), (L1_1, "high_network_0", "300ps") )
# C1_L1toL2 = sst.Link("C1_L1toL2")
# C1_L1toL2.connect( (L1_1, "low_network_0", "300ps"), (L2_1, "high_network_0", "300ps") )
#
#   End Core 1
#


#
#   Begin Core 2
#
# core2 = sst.Component("core2", "miranda.BaseCPU")
# core2.addParams({
#     "verbose" : 1,
#     "clock" : "2GHz",
#     "generator" : "miranda.STREAMBenchGenerator"
# })
# gen = core2.setSubComponent("gen2", "miranda.STREAMBenchGenerator")
# gen.addParams({
#     "generatorParams.verbose" : 1
# })
#
# L1_2 = sst.Component("L1_2", "memHierarchy.Cache")
# L1_2.addParams({
#     "cache_frequency": "2GHz",
#     "cache_size": "32KB",
#     "associativity": 8,
#     "access_latency_cycles": 4,
#     "L1": 1
# })
#
# L2_2 = sst.Component("L2_2", "memHierarchy.Cache")
# L2_2.addParams({
#     "prefetcher": "cassini.StridePrefetcher",
#     "prefetcher.reach": 16,
#     "prefetcher.detect_range" : 1,
#     "cache_frequency": "2GHz",
#     "cache_size": "256KB",
#     "associativity": 8,
#     "access_latency_cycles": 6,
#     "mshr_num_entries" : 16,
#     "mshr_latency_cycles": 2,
#     "memNIC.network_bw": "51.2GB/s"
# })
#
#
# C2toL1 = sst.Link("C2toL1")
# C2toL1.connect( (core2, "cache_link", "300ps"), (L1_2, "high_network_0", "300ps") )
# C2_L1toL2 = sst.Link("C2_L1toL2")
# C2_L1toL2.connect( (L1_2, "low_network_0", "300ps"), (L2_2, "high_network_0", "300ps") )
# End Core 2

#
# Begin Router 1
#
# rtr1 = sst.Component("rtr1", "merlin.hr_router")
# rtr1.addParams({
#     "id": 0,
#     "output_latency": "25ps",
#     "xbar_bw": "51.2GB/s",
#     "input_buf_size": "2KB",
#     "input_latency": "25ps",
#     "num_ports": 5,
#     "flit_size": "512B",
#     "output_buf_size": "2KB",
#     "link_bw": "51.2GB/s"
# })
# top1 = rtr1.setSubComponent("topology", "merlin.torus")
# top1.addParams({
#     "shape": 2,
#     "local_ports": 3,
#     "width": 1
# })
# End router 1

#
# Begin Router 2
#
# rtr2 = sst.Component("rtr2", "merlin.hr_router")
# rtr2.addParams({
#     "id": 1,
#     "output_latency": "25ps",
#     "xbar_bw": "51.2GB/s",
#     "input_buf_size": "2KB",
#     "input_latency": "25ps",
#     "num_ports": 5,
#     "flit_size": "512B",
#     "output_buf_size": "2KB",
#     "link_bw": "51.2GB/s"
# })
# top2 = rtr2.setSubComponent("topology", "merlin.torus")
# top2.addParams({
#     "shape": 2,
#     "local_ports": 3,
#     "width": 1
# })
# End router 2

# C1_CtoRtr = sst.Link("C1_CtoRtr")
# C1_CtoRtr.connect((L2_1, "directory", "300ps"), (rtr1, "port2", "300ps"))
#
# C2_CtoRtr = sst.Link("C2_CtoRtr")
# C2_CtoRtr.connect((L2_2, "directory", "300ps"), (rtr1, "port3", "300ps"))
#
# C1toC2_h = sst.Link("C1toC2_h")
# C1toC2_h.connect((rtr1, "port0", "300ps"), (rtr2, "port1", "300ps"))
#
# C2toC1_h = sst.Link("C2toC1_h")
# C2toC1_h.connect((rtr2, "port0", "300ps"), (rtr1, "port1", "300ps"))

#   Memory Director and Controller
#
dc1 = sst.Component("dc1", "memHierarchy.DirectoryController")
dc1.addParams({

})

mc1 = sst.Component("mc1", "memHierarchy.MemController")
mc1.addParams({
    "clock": "200MHz",
    "network_bw": "100MB/s",
    "backing": "malloc",
    "addr_range_start": 0,
    "backend.mem_size": "64GiB"
})
# End MC and DC components

bus = sst.Component("bus", "multiBus.MultiBus")
bus.addParams({
    "bus_frequency": "4GHz",
    "debug": 1,
    "debug_level": 10
})

# C1_CtoBus = sst.Link("C1_CtoBus")
# C1_CtoBus.connect((L2_1, "directory", "300ps"), (bus, "port0", "300ps"))
#
# C2_CtoBus = sst.Link("C2_CtoBus")
# C2_CtoBus.connect((L2_2, "directory", "300ps"), (bus, "port1", "300ps"))

# R2toDC = sst.Link("R2toDC")
# R2toDC.connect((rtr2, "port2", "300ps"), (bus, "port0", "300ps"))

BtoDC = sst.Link("BtoDC")
BtoDC.connect((bus, "port2", "300ps"), (dc1, "network", "300ps"))

DCtoMC = sst.Link("DCtoMC")
DCtoMC.connect((dc1, "memory", "300ps"), (mc1, "direct_link", "300ps"))
