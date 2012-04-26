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
public:
    Directory(int,int,int,int);
    
    void Read(ulong addr, int requestor);
    void Flush(ulong addr, int updator);
    void Upgr(ulong addr, int upgrader);
};

#endif
