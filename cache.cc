/*******************************************************
                          cache.cc
                  Ahmad Samih & Yan Solihin
                           2009
                {aasamih,solihin}@ece.ncsu.edu
********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cache.h"
using namespace std;

Cache::Cache(int s, int a, int b, int n)
{
    id = n;
    ulong i, j;
    reads = readMisses = writes = 0;
    writeMisses = writeBacks = currentCycle = 0;
    transactions = linesReceived = 0;
   
    size       = (ulong)(s);
    lineSize   = (ulong)(b);
    assoc      = (ulong)(a);   
    sets       = (ulong)((s/b)/a);
    numLines   = (ulong)(s/b);
    log2Sets   = (ulong)(log2(sets));   
    log2Blk    = (ulong)(log2(b));
  
    //*******************//
    //initialize your counters here//
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
	    cache[i][j].invalidate();
	}
    }      
}

void Cache::Access(ulong addr, uchar op, Bus* bus) { cout << "All your base are belong to us!\n"; }

/**you might add other parameters to Access()
   since this function is an entry point 
   to the memory hierarchy (i.e. caches)**/
void Cache::Access(ulong addr,uchar op)
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
	if(op == 'w') newline->setFlags(DIRTY);    
		
    }
    else
    {
	/**since it's a hit, update LRU and update dirty flag**/
	updateLRU(line);
	if(op == 'w') line->setFlags(DIRTY);
    }
}

/*look up line*/
cacheLine * Cache::findLine(ulong addr)
{
    ulong i, j, tag, pos;
   
    pos = assoc;
    tag = calcTag(addr);
    i   = calcIndex(addr);
  
    for(j=0; j<assoc; j++)
	if(cache[i][j].isValid())
	    if(cache[i][j].getTag() == tag)
	    {
		pos = j; break; 
	    }
    if(pos == assoc)
	return NULL;
    else
	return &(cache[i][pos]); 
}

/*upgrade LRU line to be MRU line*/
void Cache::updateLRU(cacheLine *line)
{
    line->setSeq(currentCycle);  
}

/*return an invalid line as LRU, if any, otherwise return LRU line*/
cacheLine * Cache::getLRU(ulong addr)
{
    ulong i, j, victim, min;

    victim = assoc;
    min    = currentCycle;
    i      = calcIndex(addr);
   
    for(j=0;j<assoc;j++)
    {
	if(cache[i][j].isValid() == 0) return &(cache[i][j]);     
    }   
    for(j=0;j<assoc;j++)
    {
	if(cache[i][j].getSeq() <= min) { victim = j; min = cache[i][j].getSeq();}
    } 
    assert(victim != assoc);
   
    return &(cache[i][victim]);
}

/*find a victim, move it to MRU position*/
cacheLine *Cache::findLineToReplace(ulong addr)
{
    cacheLine * victim = getLRU(addr);
    updateLRU(victim);
  
    return (victim);
}

/*allocate a new line*/
cacheLine *Cache::fillLine(ulong addr)
{ 
    ulong tag;
  
    cacheLine *victim = findLineToReplace(addr);
    assert(victim != 0);
    if(victim->getFlags() == DIRTY || victim->getFlags() == SHARED_DIRTY) writeBack(addr);
    	
    tag = calcTag(addr);   
    victim->setTag(tag);

    /**note that this cache line has been already 
       upgraded to MRU in the previous function (findLineToReplace)**/

    return victim;
}

void Cache::printStats(int id)
{ 
    printf("===== Simulation results (Cache_%d)      =====\n", id);
    cout << "01. number of reads:                              " << reads << "\n";
    cout << "02. number of read misses:                        " << readMisses << "\n";
    cout << "03. number of writes:                             " << writes << "\n";
    cout << "04. number of write misses:                       " << writeMisses << "\n";
    printf("05. total miss rate:                              %1.6f\n", (float)(readMisses + writeMisses)/(reads + writes));
    cout << "06. number of writebacks:                         " << writeBacks << "\n";
    cout << "07. number of memory transactions:                " << transactions << "\n";
    cout << "08. number of cache to cache transfers:           " << linesReceived << "\n";
}

//network transactions
void Cache::Flush(ulong addr) {}
void Cache::Upgr(ulong addr) {}
void Cache::WB_Int(ulong addr, int ids) {}
void Cache::ReplyId(ulong addr, int ids) {}
void Cache::ReplyD(ulong addr) {}
void Cache::InvAck(ulong addr) {}
