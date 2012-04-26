//
//  mesi.h
//
//  Created by Adam Alford & Samuel Christie
//

#ifndef __MESI_H__
#define __MESI_H__

#include "cache.h"

// add enumeration for mesi states
typedef enum {
    DRAGON_INVALID,
    DRAGON_EXCLUSIVE,
    DRAGON_SHARED,
    DRAGON_MODIFIED
} mesiStates_t;

class MesiCache : public Cache
{
public:
    MesiCache(int,int,int,Directory *);
    virtual ~MesiCache() {delete cache;}
    
    // overload only those functions that change from the base cache
    void Access(ulong,uchar);
    cacheLine * findLine(ulong addr);
    cacheLine *fillLine(ulong addr);
    
    int handleNetworkInv(ulong addr, Cache *requestor);
    ulong handleNetworkWBInv(ulong addr, Cache *requestor);
    ulong handleNetworkWBInt(ulong addr, Cache *requestor);
    ulong handleNetworkFlush(ulong addr, Cache *requestor);
};

#endif
