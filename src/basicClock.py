
#
# basicClock.py
#

import os
import sst

clockComponent0 = sst.Component("ClockComponent0", "basicComponent.basicClock")
clockComponent0.addParams({
  "clockFreq"  : "1.5Ghz",
  "clockTicks" : 1000
})

msgComp0 = clockComponent0.setSubComponent("msg", "basicComponent.basicMsg")
msgComp0.addParams({
  "clockFreq"  : "1.5Ghz",
  "count" : 10
})

clockComponent1 = sst.Component("ClockComponent1", "basicComponent.basicClock")
clockComponent1.addParams({
  "clockFreq"  : "1.5Ghz",
  "clockTicks" : 1000
})

msgComp1 = clockComponent1.setSubComponent("msg", "basicComponent.basicMsg")
msgComp1.addParams({
  "clockFreq"  : "1.5Ghz",
  "count" : 10
})

compLink = sst.Link("link0")
compLink.connect( (msgComp0, "msgPort", "1ns"), (msgComp1, "msgPort", "1ns") )

sst.setStatisticLoadLevel(3)
sst.setStatisticOutput("sst.statOutputCSV")
sst.enableAllStatisticsForComponentType("basicComponent.basicClock")
