#ifndef MULTIBUS_UTIL_H
#define	MULTIBUS_UTIL_H

#include <sst/core/stringize.h>
#include <sst/core/params.h>
#include <string>

using namespace std;

namespace SST {
    namespace MultiBus {

/* Debug macros */
#ifdef __SST_DEBUG_OUTPUT__ /* From sst-core, enable with --enable-debug */
        #define is_debug_addr(addr) (DEBUG_ADDR.empty() || DEBUG_ADDR.find(addr) != DEBUG_ADDR.end())
#define is_debug_event(ev) (DEBUG_ADDR.empty() || ev->doDebug(DEBUG_ADDR))
#define is_debug true
#else
#define is_debug_addr(addr) false
#define is_debug_event(ev) false
#define is_debug false
#endif

#define _INFO_ CALL_INFO,1,0
#define _L2_ CALL_INFO,2,0      //Debug notes, potential error warnings, etc.
#define _L3_ CALL_INFO,3,0      //External events in
#define _L4_ CALL_INFO,4,0      //External events out
#define _L5_ CALL_INFO,5,0      //Internal state transitions (e.g., coherence)
#define _L6_ CALL_INFO,6,0      //Additional detail
#define _L7_ CALL_INFO,7,0      //Additional detail
#define _L8_ CALL_INFO,8,0      //Additional detail
#define _L9_ CALL_INFO,9,0      //Additional detail
#define _L10_ CALL_INFO,10,0    //Initialization phase
#define _L11_ CALL_INFO,11,0    //Data values
#define _L20_ CALL_INFO,20,0    //Debug at function call granularity

        typedef uint64_t Addr;

    }
}
#endif	/* UTIL_H */
