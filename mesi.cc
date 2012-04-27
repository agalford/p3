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
                dir->Upgr(addr, id);
                
                // post a memory read
                memRead();
				
                // transition to S
                newline->setFlags(MODIFIED);
            }
            else if(copies_exist == 2)
            {
                // 8. I-->M
                
                // post a directory read
                dir->Upgr(addr, id);
                
                // post cache to cache read
                cacheToCacheRead();
                
                // transition to M
                newline->setFlags(MODIFIED);
            }
        }
        else // read miss
        {
            if(copies_exist == 2)
            {
                // 5. I-->S 

                // post a directory read
                dir->postRead(addr, this);
				
                //post a cache to cache read				
                cacheToCacheRead();

                // transition to S
                newline->setFlags(SHARED);
            } 
            else if(copies_exist == 1)
            {
                // 5. I-->S 

                // post a directory read
                dir ->Read(addr, id);

                // post a memory read
                memRead();
                
                // transition to E
                newline->setFlags(SHARED);
            }
            else//copies_exist == 0
            {
                // 7. I-->E

                // post a directory read
                dir->Read(addr, id);

                    // post a memory read
                    memRead();
                
                // transition to E
                newline->setFlags(EXCLUSIVE);
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
        case EXCLUSIVE:
            if(op == 'w')
                // 9. E-->M
                line->setFlags(MODIFIED);
            else
                // 8. E-->E
                ;
                
            break;
                
        case MODIFIED:
            // 1., 2. M-->M on PrRd and PrWr
                
            break;
            
        case SHARED:
            if(op == 'w')
                    
                if(copies_exist == 1)
                {
                    // 3. S-->M
                        
                    // post upgr to home
                    dir->postUpgr(addr, 0, this);

                    //RecieveInvAck
                    //does this need to be accounted for?
                        
                    // transition to M
                    line->setFlags(MODIFIED);
                }
                else if (copies_exist == 0)
                {
                    // transition to M
                    line->setFlags(MODIFIED);
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

void MesiCache::Flush(ulong addr)
{
    //find if a line contains this block
    cacheLine *line = findLine(addr);
    //printf("handling BusRd\n");
    
    if(line == NULL)
        return;

    //No cache to cache communication, return everything to home node
}
