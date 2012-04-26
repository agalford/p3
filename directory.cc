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

//
// One method to implement each possible network transaction, using processor id to indicate target. Basically just acts as a collection of caches to propagate messages.
//

void Directory::Read(ulong addr, int id)
{
}

void Directory::Flush(ulong addr)
{
}

void Directory::Upgr(ulong addr, int id)
{
}

void Directory::Flush(ulong addr, int id)
{
    caches[id]->Flush(addr);
}

void Directory::WB_Int(ulong addr, int id)
{
    caches[id]->WB_Int(addr, id);
}

void Directory::ReplyId(ulong addr, int id, int ids)
{
    caches[id]->ReplyId(addr, ids);
}

void Directory::ReplyD(ulong addr, int id)
{
    caches[id]->ReplyD(addr);
}

void Directory::InvAck(ulong addr, int id)
{
    caches[id]->InvAck(addr);
}
