//
//  mesi.cc
//
//  Created by Adam Alford & Samuel Christie
//

#include <assert.h>
#include <stdio.h>

#include "mesi.h"
#include "directory.h"

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
	if(line == NULL)/*miss*/
	{
		if(op == 'w') writeMisses++;
		else readMisses++;
        
		cacheLine *newline = fillLine(addr);
        
        if(op == 'w')
        {          
			// Send Upgr request to the directory
            ((Directory *)dir)->Upgr(addr, this);
        }
        else // read miss
        {
			// Send Upgr request to the directory
			((Directory)dir)-->Read(addr, this)
        }
	}
	else
	{
		/**since it's a hit, update LRU and update dirty flag**/
		updateLRU(line);
        
        ulong flags = line->getFlags();
        
        switch(flags)
        {
            case MESI_EXCLUSIVE:
                if(op == 'w')
					line->setFlags(MESI_MODIFIED);
                else
                    ;
                
                break;
                
            case MESI_MODIFIED:
                // 1., 2. M-->M on PrRd and PrWr
                
                break;
            
            case MESI_SHARED:
                if(op == 'w')
                    // post upgr to home
                    ((Directory *)dir)->Upgr(addr, this);
                else
                    // 4. S->S on PrRd
                    ;
                
                break;
            
            default:
                break;
        }
    }
}

/*allocate a new line*/
cacheLine *MesiCache::fillLine(ulong addr)
{ 
    ulong tag;
    
    cacheLine *victim = findLineToReplace(addr);
    assert(victim != 0);
    
    if(victim->getFlags() == MODIFIED)
    {
        writeBack(addr);
        memWrite();
    }
    
    tag = calcTag(addr);  
    victim->setTag(tag);
    // other code will set the flags.
    /**note that this cache line has been already 
     upgraded to MRU in the previous function (findLineToReplace)**/
    
    return victim;
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

void MesiCache::WB_Int(ulong addr)
{
    // find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return;

    line->setFlags(SHARED);
}

void MesiCache::ReplyD(ulong addr, bool shared)
{
	// find if a line contains this block
    cacheLine *line = findLine(addr);
    
	if(shared)
	{
		line->setFlags(MESI_SHARED);
	}
	else
	{
		line->setFlags(MESI_EXCLUSIVE);
	}
}

void MesiCache::Flush(ulong addr, Cache *requestor)
{
	// find if a line contains this block
    cacheLine *line = findLine(addr);

	//flush to requestor
    requestor->Flush(line);

	cacheToCacheWrites++;
}
