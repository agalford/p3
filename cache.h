/*******************************************************
                          cache.h
                  Ahmad Samih & Yan Solihin
                           2009
                {aasamih,solihin}@ece.ncsu.edu
********************************************************/

#ifndef CACHE_H
#define CACHE_H

#include <cmath>
#include <iostream>

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;

/****add new states, based on the protocol****/
enum{
    INVALID=0,
    SHARED,
    EXCLUSIVE,
    MODIFIED,
    EM,
    UNOWNED
};

class Bus;

class cacheLine 
{
 protected:
    ulong tag;
    ulong Flags;
    ulong seq; 
 
 public:
    int fbv;
    cacheLine()            { tag = 0; Flags = 0; }
    ulong getTag()         { return tag; }
    ulong getFlags()			{ return Flags;}
    ulong getSeq()         { return seq; }
    void setSeq(ulong Seq)			{ seq = Seq;}
    void setFlags(ulong flags)			{  Flags = flags;}
    void setTag(ulong a)   { tag = a; }
    void invalidate()      { tag = 0; Flags = INVALID; }//useful function
    bool isValid()         { return ((Flags) != INVALID); }
};

class Cache
{
 protected:
    int id;
    ulong size, lineSize, assoc, sets, log2Sets, log2Blk, tagMask, numLines;
    ulong reads,readMisses,writes,writeMisses,writeBacks,transactions,linesReceived;

    //******///
    //add coherence counters here///
    //******///

    cacheLine **cache;
    ulong calcTag(ulong addr)     { return (addr >> (log2Blk) );}
    ulong calcIndex(ulong addr)  { return ((addr >> log2Blk) & tagMask);}
    ulong calcAddr4Tag(ulong tag)   { return (tag << (log2Blk));}
   
 public:
    ulong currentCycle;  
    
    Cache(int,int,int,int);
    virtual ~Cache() { delete cache;}
   
    virtual cacheLine *findLineToReplace(ulong addr);
    cacheLine *fillLine(ulong addr);
    cacheLine * findLine(ulong addr);
    cacheLine * getLRU(ulong);
   
    ulong getRM(){return readMisses;} ulong getWM(){return writeMisses;} 
    ulong getReads(){return reads;}ulong getWrites(){return writes;}
    ulong getWB(){return writeBacks;}
   
    void writeBack(ulong)   {writeBacks++; transactions++;}
    virtual void Access(ulong,uchar);
    virtual void Access(ulong,uchar,Bus*);
    void printStats(int);
    void updateLRU(cacheLine *);

    //network transactions
    void Read(ulong addr);
    void Flush(ulong addr);
    void Upgr(ulong addr);
    void WB_Int(ulong addr, int ids);
    void ReplyId(ulong addr, int ids);
    void Reply(ulong addr);
    void ReplyD(ulong addr);
    void Inv(ulong addr);
};

#endif
