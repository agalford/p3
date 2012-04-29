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

    int getId(int fbv);

 public:
    Directory(Cache**, int, int, int, int);

    cacheLine* fillLine(ulong addr);

    //network transactions
    void Read(ulong addr, int id);
    void Flush(ulong addr, int id);
    void Upgr(ulong addr, int id);
    void Disown(ulong addr, int id);
};

#endif
