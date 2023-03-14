#import sst
#import sys

# Define SST core options
sst.setProgramOption("timebase", "1 ps")

#-- set the "stop-at" option
sst.setProgramOption("stop-at", "10s")


#
#   Begin Core 1
#
cpu = sst.Component("cpu", "miranda.BaseCPU")
cpu.addParams({
    "verbose" : 0,
    "clock" : "2GHz",
    "printStats" : 1,
})
gen = cpu.setSubComponent("generator", "miranda.Stencil3DBenchGenerator")
gen.addParams({
    "verbose" : 0,
    "nx" : 30,
    "ny" : 20,
    "nz" : 10,
})

L1 = sst.Component("L1", "memHierarchy.Cache")
L1.addParams({
    "access_latency_cycles" : "2",
    "cache_frequency" : "2 GHz",
    "replacement_policy" : "lru",
    "coherence_protocol" : "MESI",
    "associativity" : "4",
    "cache_line_size" : "64",
    "prefetcher" : "cassini.StridePrefetcher",
    "debug" : "0",
    "L1" : "1",
    "cache_size" : "32KB"
})

L2 = sst.Component("L2", "memHierarchy.Cache")
L2.addParams({
    "access_latency_cycles" : "9",
    "cache_frequency" : "2Ghz",
    "replacement_policy" : "lru",
    "coherence_protocol" : "MESI",
    "associativity" : "8",
    "cache_line_size" : "64",
    "cache_size" : "32 KB",
    "debug" : "0"
})

mc = sst.Component("mc", "memHierarchy.MemController")
mc.addParams({
    "debug" : 10,
    "debug_level" : 10,
    "clock" : "1GHz",
    "addr_range_end" : 512*1024*1024-1,
})

memory = mc.setSubComponent("backend", "memHierarchy.simpleMem")
memory.addParams({
    "mem_size" : "512MiB",
    "access_time" : "100 ns",
})

# Create the link to the cache hierarhcy
iface = comp_cpu.setSubComponent("memory", "memHierarchy.standardInterface")
link_cpu_cache = sst.Link("link_cpu_cache")
link_cpu_cache.connect( (iface, "port", "1000ps"), (L1, "high_network_0", "1000ps") )
link_cpu_cache.setNoCut()

link_cpu_l1_l2 = sst.Link("link_l1_l2")
link_cpu_l1_l2.connect( (L1, "low_network_0", "1000ps"), (L2, "high_network_0", "1000ps") )

link_mem_bus_link = sst.Link("link_l2_mem")
link_mem_bus_link.connect( (L2, "low_network_0", "10000ps"), (mc, "direct_link", "10000ps") )
