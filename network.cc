//
//  network.cc
//  Acts as a network controller to perform network transactions
//
//  Created by Adam Alford & Samuel Christie
//

Network::Network(Cache** cs, int n)
{
    caches = cs;
    num_caches = n;
}

//
// One method to implement each possible network transaction, using processor id to indicate target. Basically just acts as a collection of caches to propagate messages.
//

void Network::Read(ulong addr, int id)
{
    caches[i]->Read(addr);
}

void Network::Flush(ulong addr, int id)
{
    caches[i]->Flush(addr);
}

void Network::Upgr(ulong addr, int id)
{
    caches[i]->Upgr(addr);
}

void Network::WB_Int(ulong addr, int id)
{
    caches[i]->WB_Int(addr);
}

void Network::ReplyId(ulong addr, int id)
{
    caches[i]->ReplyId(addr);
}

void Network::Reply(ulong addr, int id)
{
    caches[i]->Reply(addr);
}

void Network::ReplyD(ulong addr, int id)
{
    caches[i]->ReplyD(addr);
}
