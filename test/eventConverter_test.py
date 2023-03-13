#import sst
#import sys

# Define SST core options
sst.setProgramOption("timebase", "1 ps")

#-- set the "stop-at" option
sst.setProgramOption("stop-at", "10s")

#-- set statistics level and output
sst.setStatisticLoadLevel(1)
sst.setStatisticOutput("sst.statOutputConsole")
sst.setStatisticOutputOptions({
    "filepath": "stats.txt",
    "separator": "\t",
})

# Create the components
cpu = sst.Component("cpu", "miranda.BaseCPU")
cache = sst.Component("cache", "memHierarchy.Cache")
# router = sst.Component("router", "merlin.hr_router")
# myCmp1 = sst.Component("myComp1", "myComponent.MyComponent")
# myCmp2 = sst.Component("myComp2", "myComponent.MyComponent")
md = sst.Component("memDir", "memHierarchy.DirectoryController")
mc = sst.Component("memCon", "memHierarchy.MemController")
# Set the parameters

cpu.addParams({
    "verbose" : 1,
    "clock" : "2GHz",
    "generator" : "miranda.STREAMBenchGenerator"
})
gen = cpu.setSubComponent("gen", "miranda.STREAMBenchGenerator")
gen.addParams({
    "generatorParams.verbose" : 1
})

cache.addParams({
    "L1": 1,
    "cache_type": "inclusive",
    "cache_frequency": "2GHz",
    "cache_size": "64KiB",
    "cache_line_size": 64,
    "coherence_protocol": "MESI",
    "banks": 8,
    "associativity": 8,
    "access_latency_cycles": "4",
    "node": 1
})

# router.addParams({
#     "id": 0,
#     "output_latency": "25ps",
#     "xbar_bw": "51.2GB/s",
#     "input_buf_size": "2KB",
#     "input_latency": "25ps",
#     "num_ports": 4,
#     "flit_size": "512B",
#     "output_buf_size": "2KB",
#     "link_bw": "51.2GB/s"
# })
# rtrTop = router.setSubComponent("topology", "merlin.torus")
# rtrTop.addParams({
#     "shape": 1,
#     "local_ports": 2,
#     "width": 1
# })

mc.addParams({
    "clock": "200MHz",
    "network_bw": "100MB/s",
    "backing": "malloc",
    "addr_range_start": 0,
    "backend.mem_size": "64GiB"
})

# Connect the ports
# link0 = sst.Link("link0")
# link0.connect((router, "port2", "100ps"), (router, "port3", "50ps"))

link1 = sst.Link("link1")
link1.connect((cpu, "cache_link", "100ps"), (cache, "high_network_0", "50ps"))

link2 = sst.Link("link2")
link2.connect((cache, "directory", "100ps"), (md, "network", "50ps"))

# link3 = sst.Link("link3")
# link3.connect((myCmp1, "network", "100ps"), (router, "port0", "50ps"))
#
# link4 = sst.Link("link4")
# link4.connect((myCmp2, "network", "100ps"), (router, "port1", "50ps"))
#
# link3 = sst.Link("link3")
# link3.connect((myCmp1, "network", "100ps"), (myCmp2, "network", "50ps"))
#
# link5 = sst.Link("link5")
# link5.connect((myCmp2, "memory", "100ps"), (md, "network", "50ps"))

link6 = sst.Link("link6")
link6.connect((md, "memory", "100ps"), (mc, "direct_link", "50ps"))
