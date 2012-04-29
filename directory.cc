//
//  directory.cc
//
//  Created by Adam Alford & Samuel Christie
//

#include <stdio.h>
#include <iostream>
#include <assert.h>
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
        if (fbv & (1<<i)) {
            return i;
        }
    }
    return -1;
}

/*allocate a new line*/
cacheLine *Directory::fillLine(ulong addr)
{ 
    ulong tag;
  
    cacheLine *victim = findLineToReplace(addr);
    assert(victim != 0);
    if(victim->getFlags() == MODIFIED) {
        writeBack(addr);
    }

    tag = calcTag(addr);   
    victim->setTag(tag);

    /**note that this cache line has been already 
       upgraded to MRU in the previous function (findLineToReplace)**/

    return victim;
}

//
// One method to implement each possible network transaction, using processor id to indicate target. Basically just acts as a collection of caches to propagate messages.
//

void Directory::Read(ulong addr, int id)
{
    cacheLine * line = findLine(addr);
    if (line == NULL) {
        line = fillLine(addr);
        line->setFlags(EM);
        line->fbv = 1<<id; //new line, so just set the FBV
        caches[id]->ReplyD(addr, false); //not shared, reply from memory
    } else {
        updateLRU(line);
        if (line->getFlags() == SHARED) {
            //shared, so directory has the cache data
            caches[id]->ReplyD(addr, true); //shared, reply from directory cache
        } else if (line->getFlags() == EM) {
            //could be modified, have the owner flush
            caches[getId(line->fbv)]->WB_Int(addr, id);
            line->setFlags(SHARED);
        }

        line->fbv |= (1<<id); //add cache to set of owners
    }
}

void Directory::Upgr(ulong addr, int id)
{
    cacheLine * line = findLine(addr);
    if (line == NULL)
        line = fillLine(addr);

    for(int i=0; i<num_caches; i++) {
        if (i != id) {
            caches[i]->Inv(addr); //invalidate other caches
        }
    }

    line->setFlags(EM);
    line->fbv = 1<<id; //line is now owned by a single processor, set the fbv
}

void Directory::Flush(ulong addr, int id)
{
    caches[id]->Flush(addr);
}

void Directory::Disown(ulong addr, int id)
{
    cacheLine* line = findLine(addr);
    if (line!=NULL) {
        line->fbv &= ~(1<<id); // remove id from the list of owners
        if (line->fbv == 0)
            line->setFlags(INVALID);
    }
}
