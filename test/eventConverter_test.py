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

#
# Begin Router 1
#
rtr1 = sst.Component("rtr1", "merlin.hr_router")
rtr1.addParams({
    "id": 0,
    "output_latency": "25ps",
    "xbar_bw": "51.2GB/s",
    "input_buf_size": "2KB",
    "input_latency": "25ps",
    "num_ports": 5,
    "flit_size": "512B",
    "output_buf_size": "2KB",
    "link_bw": "51.2GB/s"
})
top1 = rtr1.setSubComponent("topology", "merlin.torus")
top1.addParams({
    "shape": 2,
    "local_ports": 3,
    "width": 1
})
# End router 1

#
# Begin Router 2
#
rtr2 = sst.Component("rtr2", "merlin.hr_router")
rtr2.addParams({
    "id": 1,
    "output_latency": "25ps",
    "xbar_bw": "51.2GB/s",
    "input_buf_size": "2KB",
    "input_latency": "25ps",
    "num_ports": 3,
    "flit_size": "512B",
    "output_buf_size": "2KB",
    "link_bw": "51.2GB/s"
})
top2 = rtr2.setSubComponent("topology", "merlin.torus")
top2.addParams({
    "shape": 2,
    "local_ports": 1,
    "width": 1
})
# End router 2

# Create the link to the cache hierarhcy
iface = cpu.setSubComponent("memory", "memHierarchy.standardInterface")
link_cpu_cache = sst.Link("link_cpu_cache")
link_cpu_cache.connect( (iface, "port", "1000ps"), (L1, "high_network_0", "1000ps") )
link_cpu_cache.setNoCut()

link_cpu_l1_l2 = sst.Link("link_l1_l2")
link_cpu_l1_l2.connect( (L1, "low_network_0", "1000ps"), (L2, "high_network_0", "1000ps") )

link_rtr1_rtr2 = sst.Link("link_rtr1_rtr2")
link_rtr1_rtr2.connect( (rtr1, "port0", "1000ps"), (rtr2, "port1", "1000ps") )

link_rtr2_rtr1 = sst.Link("link_rtr2_rtr1")
link_rtr2_rtr1.connect( (rtr2, "port0", "1000ps"), (rtr1, "port1", "1000ps") )

link_cpu_l2_rtr1 = sst.Link("link_cpu_l2_rtr1")
link_cpu_l2_rtr1.connect( (L2, "low_network_0", "10000ps"), (rtr1, "port2", "10000ps") )
link_cpu_l2_rtr1.setNoCut()

link_rtr2_mem = sst.Link("link_rtr2_mem")
link_rtr2_mem.connect( (rtr2, "port2", "10000ps"), (mc, "direct_link", "10000ps") )
