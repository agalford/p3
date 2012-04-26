//
//  mesi.cc
//
//  Created by Adam Alford & Samuel Christie
//

#include <assert.h>
#include <stdio.h>

#include "mesi.h"
#include "directory.h"


MesiCache::MesiCache(int s,int a,int b, Directory *newDirectory )
{
    ulong i, j;
    reads = readMisses = writes = 0; 
    writeMisses = writeBacks = currentCycle = 0;
	invalidations = 0;
    
    size       = (ulong)(s);
    lineSize   = (ulong)(b);
    assoc      = (ulong)(a);   
    sets       = (ulong)((s/b)/a);
    numLines   = (ulong)(s/b);
    log2Sets   = (ulong)(log2(sets));   
    log2Blk    = (ulong)(log2(b));   
    
    //*******************//
    //initialize your counters here//
    memReads = memWrites = 0;
    cacheToCacheReads = cacheToCacheWrites = 0;
    //*******************//

    
    tagMask =0;
    for(i=0;i<log2Sets;i++)
    {
		tagMask <<= 1;
        tagMask |= 1;
    }
    
    /**create a two dimentional cache, sized as cache[sets][assoc]**/ 
    cache = new cacheLine*[sets];
    for(i=0; i<sets; i++)
    {
        cache[i] = new cacheLine[assoc];
        for(j=0; j<assoc; j++) 
        {
            cache[i][j].setTag(MESI_INVALID);
        }
    }
    
    dir = newDirectory;
    
    cacheNumber = 0;
    
}

/*look up line*/
cacheLine * MesiCache::findLine(ulong addr)
{
    ulong i, j, tag, pos;
    
    pos = assoc;
    tag = calcTag(addr);
    i   = calcIndex(addr);
    
    for(j=0; j<assoc; j++)
        if(cache[i][j].isValid())
		{
	        if(cache[i][j].getTag() == tag)
            {
                pos = j; break; 
            }
		}
    if(pos == assoc)
        return NULL;
    else
        return &(cache[i][pos]); 
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
	
    int copies_exist = dir->checkCopiesExist(addr, this);
	//0-no copies
	//1-shared copy
	//2-EM copy
    
	cacheLine * line = findLine(addr);
	if(line == NULL)/*miss*/
	{
		if(op == 'w') writeMisses++;
		else readMisses++;
        
		cacheLine *newline = fillLine(addr);
   		
        //if(op == 'w') newline->setFlags(DIRTY);
        
        if(op == 'w')
        {
            if(copies_exist == 0 || copies_exist == 1)
            {
                // 6. I-->M
                                
				//should this be an ReadX request?
                ((Directory *)dir)->postUpgr(addr, this);
                
				// post a memory read
				memRead();
				
                // transition to S
                newline->setFlags(MESI_MODIFIED);
            }
            else if(copies_exist == 2)
            {
                // 8. I-->M
                
                // post a directory read
                ((Bus *)bus)->postUpgr(addr, this);
                
                // post cache to cache read
                cacheToCacheRead();
                
                // transition to M
                newline->setFlags(MESI_MODIFIED);
            }
        }
        else // read miss
        {
            if(copies_exist == 2)
            {
				// 5. I-->S 

				// post a directory read
				((Directory)dir)-->postRead(addr, this)
				
				//post a cache to cache read				
				cacheToCacheRead();

				// transition to S
                newline->setFlags(MESI_SHARED);
            } 
            else if(copies_exist == 1)
            {
				// 5. I-->S 

				// post a directory read
				((Directory)dir)-->postRead(addr, this)

				// post a memory read
                memRead();
                
                // transition to E
                newline->setFlags(MESI_SHARED);
            }
			else//copies_exist == 0
			{
				// 7. I-->E

				// post a directory read
				((Directory)dir)-->postRead(addr, this)

				// post a memory read
                memRead();
                
                // transition to E
                newline->setFlags(MESI_EXCLUSIVE);
			}

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
					// 9. E-->M
					line->setFlags(MESI_MODIFIED);
                else
                    // 8. E-->E
                    ;
                
                break;
                
            case MESI_MODIFIED:
                // 1., 2. M-->M on PrRd and PrWr
                
                break;
            
            case MESI_SHARED:
                if(op == 'w')
                    
                    if(copies_exist == 1)
                    {
                        // 3. S-->M
                        
                        // post upgr to home
                        ((Directory *)dir)->postUpgr(addr, 0, this);

						//RecieveInvAck
						//does this need to be accounted for?
                        
						// transition to M
                        line->setFlags(MESI_MODIFIED);
                    }
                    else if (copies_exist == 0)
                    {
						// transition to M
                        line->setFlags(MESI_MODIFIED);
                    }
                    
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
    
    if(victim->getFlags() == MESI_MODIFIED || victim->getFlags() == MESI_SHARED_MODIFIED)
    {
        writeBack(addr);
        memWrite();
    }
    
    // FIXME: what else needs to happen here? this is basically what should happen on eviction or what? 
    tag = calcTag(addr);  
    victim->setTag(tag);
    // other code will set the flags.
    //victim->setFlags(VALID);    
    /**note that this cache line has been already 
     upgraded to MRU in the previous function (findLineToReplace)**/
    
    return victim;
}

int MesiCache::handleNetworkInv(ulong addr, Directory *requestor)
{
	// find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return -1;

	//increse invalidation count
	invalidations++;

	line->setFlags(MESI_INVALID);

	//No cache to cache communication, return everything to home node
	return 1;
}

ulong MesiCache::handleNetworkWBInv(ulong addr, Directory *requestor)
{
	// find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return -1;

	//increse invalidation count
	invalidations++;

	line->setFlags(MESI_INVALID);

	//No cache to cache communication, return everything to home node
	return line;
}

ulong MesiCache::handleNetworkWBInt(ulong addr, Directory *requestor)
{
	// find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return -1;

	line->setFlags(MESI_SHARED);

	//No cache to cache communication, return everything to home node
	return line;
}

ulong MesiCache::handleNetworkFlush(ulong addr, Directory *requestor)
{
	// find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return -1;

	//No cache to cache communication, return everything to home node
	return line;
}
