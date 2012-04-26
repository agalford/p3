//
//  directory.cc
//
//  Created by Adam Alford & Samuel Christie
//

#include <stdio.h>
#include "mesi.h"
#include "directory.h"
using namespace std;

Directory::Directory(Cache** cs, int n, int s, int a, int b) : Cache(s,a,b,0)
{
    caches = cs;
    num_caches = n;
}

int Directory::getId(int fbv) //get first id from bit vector
{
    for(int i=0; i<num_caches; i++) {
        if (fbv & (1<<i))
            return i;
    }
    return -1;
}

//
// One method to implement each possible network transaction, using processor id to indicate target. Basically just acts as a collection of caches to propagate messages.
//

void Directory::Read(ulong addr, int id)
{
    cacheLine * line = findLine(addr);
    if (line == NULL) {
        //read miss
        line = fillLine(addr);
        line->setFlags(EM);
        line->fbv = 1<<id; //new line, so just set the FBV
    } else {
        //read hit
        if (line->getFlags() == SHARED) {
            //shared, so directory has the cache data
            caches[id]->ReplyD(addr);
        } else if (line->getFlags() == EM) {
            //could be modified, have the owner do it
            caches[getId(line->fbv)]->WB_Int(addr, id);
            line->fbv |= (1<<id);
        }
    }
    caches[id]->ReplyD(addr);
}

void Directory::Flush(ulong addr)
{
    findLine(addr);
}

void Directory::Upgr(ulong addr, int id)
{
    cacheLine * line = findLine(addr);
    if (line == NULL) {
        //read miss
        line = fillLine(addr);
        line->setFlags(EM);
        line->fbv = 1<<id; //new line, so just set the FBV
    } else {
        //read hit
        caches[id]->ReplyId(addr, line->fbv);
        for(int i=0; i<num_caches; i++) {
            if (line->fbv & 1<<i) {
                caches[i]->Inv(addr);
            }
        }
    }
}

void Directory::Flush(ulong addr, int id)
{
    caches[id]->Flush(addr);
}
