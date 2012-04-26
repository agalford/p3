//
//  directory.cc
//
//  Created by Adam Alford & Samuel Christie
//

#include <stdio.h>
#include "mesi.h"
#include "directory.h"
using namespace std;

Directory::Directory(int s, int a, int b, int n) : Cache(s,a,b,n) {}

cacheLine* Directory::Read(ulong addr, int id);
cacheLine* Directory::Flush(ulong addr, int id);
cacheLine* Directory::Upgr(ulong addr, int id);

