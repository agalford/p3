//
//  mesi.h
//
//  Created by Adam Alford & Samuel Christie
//

#ifndef __MESI_H__
#define __MESI_H__

#include "cache.h"
#include "directory.h"

class MesiCache : public Cache
{
    Directory* dir;

public:
    MesiCache(int,int,int,int,Directory *);
    virtual ~MesiCache() {delete cache;}
    
    // overload only those functions that change from the base cache
    void Access(ulong,uchar);
    
    void Inv(ulong addr);
    void WB_Inv(ulong addr);
    void WB_Int(ulong addr, int id);
    void Flush(ulong addr);
    void ReplyD(ulong addr, bool shared);
};

#endif
