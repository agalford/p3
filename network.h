//
//  network.h
//  Class Network acts as a network controller
//
//  Created by Adam Alford & Samuel Christie
//

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "cache.h"


class Network
{
    Cache** caches;
    Directory* dir;
    int num_caches;
 public:
    Network(Cache**, int);

    //network transactions
    void Read(ulong addr);
    void Flush(ulong addr);
    void Upgr(ulong addr);
    void WB_Int(ulong addr);
    void ReplyId(ulong addr);
    void Reply(ulong addr);
    void ReplyD(ulong addr);
};
