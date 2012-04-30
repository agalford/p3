//
//  directory.h
//  Class Directory derived from generic Cache
//
//  Created by Adam Alford & Samuel Christie
//

#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include "cache.h"

class Directory
{
    Cache** caches;
    int num_caches;
    ulong size, num_lines, log2Blk;

    int getId(int fbv);

    cacheLine* cache;
    ulong calcTag(ulong addr)     { return (addr >> (log2Blk) );}

 public:
    Directory(Cache**, int, int, int, int);

    cacheLine* findLine(ulong addr);
    cacheLine* fillLine(ulong addr);

    //network transactions
    void Read(ulong addr, int id);
    void Flush(ulong addr, int id);
    void Upgr(ulong addr, int id);
    void Disown(ulong addr, int id);

    int getFbv(ulong addr);
    int getState(ulong addr);
};

#endif
