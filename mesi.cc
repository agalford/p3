//
//  mesi.cc
//
//  Created by Adam Alford & Samuel Christie
//

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include "mesi.h"
#include "directory.h"
using namespace std;

MesiCache::MesiCache(int s,int a,int b,int n, Directory *newDirectory ) : Cache(s,a,b,n)
{
    dir = newDirectory;
}

/**you might add other parameters to Access()
   since this function is an entry point 
   to the memory hierarchy (i.e. caches)**/
void MesiCache::Access(ulong addr,uchar op)
{
    currentCycle++;/*per cache global counter to maintain LRU order 
                     among cache ways, updated on every cache access*/
    
    if(op == 'w') writes++;
    else          reads++;

    cacheLine * line = findLine(addr);
    if(line == NULL) {/*miss*/
        if(op == 'w') writeMisses++;
        else readMisses++;
        
        if(op == 'w') {          
            // Send Upgr request to the directory
            line = fillLine(addr);
            line->setFlags(MODIFIED);
            dir->Upgr(addr, id);
        }
        else {// read miss
            // Send Upgr request to the directory
            dir->Read(addr, id);
        }
    }
    else {
        /**since it's a hit, update LRU and update dirty flag**/
        updateLRU(line);
        
        ulong flags = line->getFlags();
        
        switch(flags) {
        case EXCLUSIVE:
            if(op == 'w')
                line->setFlags(MODIFIED);
            break;
                
        case MODIFIED:
            // 1., 2. M-->M on PrRd and PrWr
            break;
            
        case SHARED:
            if(op == 'w')
                // post upgr to home
                dir->Upgr(addr, id);
            else
                // 4. S->S on PrRd
                ;
                
            break;
            
        default:
            break;
        }
    }
}

void MesiCache::Inv(ulong addr)
{
    // find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return;

    //increse invalidation count
    invalidations++;

    line->setFlags(INVALID);

    //No cache to cache communication
}

void MesiCache::WB_Inv(ulong addr)
{
    // find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return;

    //increse invalidation count
    invalidations++;

    line->setFlags(INVALID);

    //No cache to cache communication
}

void MesiCache::WB_Int(ulong addr, int id)
{
    // find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        cout << "attempting to have non-owner flush line";

    line->setFlags(SHARED);

    dir->Flush(addr); //flush to directory
    dir->Flush(addr, id); //flush to requestor
}

void MesiCache::ReplyD(ulong addr, bool shared)
{
    // find if a line contains this block, or create it if it's a new line
    cacheLine *line = findLine(addr);
    if (line==NULL)
        line = fillLine(addr);
    
    if(shared) {
        line->setFlags(SHARED);
    }
    else {
        line->setFlags(EXCLUSIVE);
    }
}

//receiving the data from another processor after requesting it
void MesiCache::Flush(ulong addr)
{
    // find if a line contains this block
    cacheLine *line = findLine(addr);
    if (line != NULL) {
        //alread have the line, so its state needs to be updated
        line->setFlags(SHARED);
    } else {
        //don't have it yet; fill the line and set to shared
        line = fillLine(addr);
        line->setFlags(SHARED);
    }
    
    //we received the data from another cache, so increment the cache-to-cache transfer counter
    linesReceived++;
}
