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
cache = sst.Component("cache", "memHierarchy.Cache")
router = sst.Component("router", "merlin.hr_router")
my_comp = sst.Component("my_comp", "MyComponent")

# Set the parameters
cache.addParams({
    "cache_frequency": "2GHz",
    "cache_size": "64KiB",
    "access_latency_cycles": "4",
})
router.addParams({
    "num_ports": "4",
    "flit_size": "8B",
    "link_bw": "1GB/s",
})
my_comp.addParams({
    "network_num_vc": "2",
})

# Connect the ports
link1 = sst.Link("link1")
link1.connect((cache, "cpu", "100ps"), (my_comp, "memory", "50ps"))
link2 = sst.Link("link2")
link2.connect((my_comp, "network", "50ps"), (router, "port", "50ps"))
