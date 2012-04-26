/*******************************************************
                          main.cc
                  Ahmad Samih & Yan Solihin
                           2009
                {aasamih,solihin}@ece.ncsu.edu

                        Modified by
              Samuel Christie (schrist@ncsu.edu)
                           2012
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iostream>
using namespace std;

#include "cache.h"
#include "mesi.h"
#include "directory.h"

int main(int argc, char *argv[])
{
	
    ifstream fin;
    FILE * pFile;

    if(argc < 7){
	printf("input format: ");
	printf("./smp_cache <cache_size> <assoc> <block_size> <trace_file>\n");
	exit(0);
    }

    int cache_size = atoi(argv[1]);
    int cache_assoc= atoi(argv[2]);
    int blk_size   = atoi(argv[3]);
    int num_processors = 4;
    const char *fname = argv[6];

    //*********************************************//
    //****Print out configuration settings here****//
    //*********************************************//	
    cout << "===== 506 DSM (MESI with Full-bit Vector Simulator Configuration =====\n";
    cout << "L1_SIZE:               " << cache_size << "\n";
    cout << "L1_ASSOC:              " << cache_assoc << "\n";
    cout << "L1_BLOCKSIZE:          " << blk_size << "\n";
    cout << "NUMBER OF PROCESSORS:  " << num_processors << "\n";
    cout << "TRACE FILE:            " << fname << "\n";

    //*********************************************//
    //*****create an array of caches here**********//
    //*********************************************//	
    Cache **caches = new Cache*[num_processors];
    Directory* dir = new Directory(caches, num_processors, cache_size, cache_assoc, blk_size);

    for (int i = 0; i < num_processors; i++) {
        caches[i] = new MesiCache(cache_size, cache_assoc, blk_size, i, dir); //initialize caches
    }

    pFile = fopen (fname,"r");
    if(pFile == 0) {
	printf("Trace file problem\n");
	exit(0);
    }

    ///******************************************************************//
    //**read trace file,line by line,each(processor#,operation,address)**//
    //*****propagate each request down through memory hierarchy**********//
    //*****by calling cachesArray[processor#]->Access(...)***************//
    ///******************************************************************//
/*
    int id;
    char op[16];
    char addr[16];
    while(!feof(pFile)) {
      int c = fscanf(pFile, "%d %s %s", &id, op, addr);
      if (c == 3)
	caches[id]->Access(strtoul(addr,NULL,16), op[0], bus);
    }
*/
	
    fclose(pFile);

    //********************************//
    //print out all caches' statistics//
    //********************************//
    for (int i=0; i < num_processors; i++) {
//	caches[i]->printStats(i);
    }
}
