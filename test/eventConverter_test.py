#import sst
#import sys

# Define SST core options
sst.setProgramOption("timebase", "1 ps")

#-- set the "stop-at" option
sst.setProgramOption("stop-at", "10s")


#
#   Begin Core 1
#
core1 = sst.Component("core1", "miranda.BaseCPU")
core1.addParams({
    "verbose" : 1,
    "clock" : "2GHz",
    "generator" : "miranda.STREAMBenchGenerator"
})
gen = core1.setSubComponent("gen1", "miranda.STREAMBenchGenerator")
gen.addParams({
    "generatorParams.verbose" : 1
})

L1_1 = sst.Component("L1_1", "memHierarchy.Cache")
L1_1.addParams({
    "cache_frequency": "2GHz",
    "cache_size": "32KB",
    "associativity": 8,
    "access_latency_cycles": 4,
    "L1": 1
})

L2_1 = sst.Component("L2_1", "memHierarchy.Cache")
L2_1.addParams({
    "prefetcher": "cassini.StridePrefetcher",
    "prefetcher.reach": 16,
    "prefetcher.detect_range" : 1,
    "cache_frequency": "2GHz",
    "cache_size": "256KB",
    "associativity": 8,
    "access_latency_cycles": 6,
    "mshr_num_entries" : 16,
    "mshr_latency_cycles": 2,
    "memNIC.network_bw": "51.2GB/s"
})


C1toL1 = sst.Link("C1toL1")
C1toL1.connect( (core1, "cache_link", "300ps"), (L1_1, "high_network_0", "300ps") )
C1_L1toL2 = sst.Link("C1_L1toL2")
C1_L1toL2.connect( (L1_1, "low_network_0", "300ps"), (L2_1, "high_network_0", "300ps") )
#
#   End Core 1
#

#
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

#myComp = sst.Component("myComp","myComponent.MyComponent")

R2toDC = sst.Link("C2toDC")
R2toDC.connect((L2_1, "directory", "300ps"), (dc1, "network", "300ps"))

#MyComptoDC = sst.Link("MyComptoDC")
#MyComptoDC.connect((myComp, "memory", "300ps"), (dc1, "network", "300ps"))

DCtoMC = sst.Link("DCtoMC")
DCtoMC.connect((dc1, "memory", "300ps"), (mc1, "direct_link", "300ps"))

