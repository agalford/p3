//
//  directory.h
//  Class Directory derived from generic Cache
//
//  Created by Adam Alford & Samuel Christie
//

#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include "cache.h"

class Directory : public Cache
{
    Cache** caches;
    int num_caches;

 public:
    Directory(Cache**, int, int, int, int);

    //network transactions
    void Read(ulong addr, int id);
    void Flush(ulong addr);
    void Flush(ulong addr, int id);
    void Upgr(ulong addr, int id);
    void WB_Int(ulong addr, int id);
    void ReplyId(ulong addr, int id, int ids);
    void ReplyD(ulong addr, int id);
    void InvAck(ulong addr, int id);
};

#endif
