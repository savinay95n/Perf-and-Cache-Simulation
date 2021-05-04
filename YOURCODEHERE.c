#include "YOURCODEHERE.h"
#include <math.h>

/*
  Sets the following fields in cache "acache"

  acache->numways : The number of ways
  acache->blocksize: The size of one block, in bytes
  acache->numsets: The number of sets in the cache
  acache->BO: The number of bits of block offset
  acache->TO: The number of bits of tag offset (i.e. the number of bits to shift the tag so that its lowest bit is at position 0)
  acache->VAImask: An andmask the size of the index field
  acache->VATmask: An andmask the size of the tag field

 */
void  setSizesOffsetsAndMaskFields(cache* acache, unsigned int size, unsigned int assoc, unsigned int blocksize){
  unsigned int localVAbits=8*sizeof(uint64_t*);
  if (localVAbits!=64){
    fprintf(stderr,"Running non-portable code on unsupported platform, terminating. Please use designated machines.\n");
    exit(-1);
  }

  // YOUR CODE GOES HERE
  acache->numways = assoc;
  acache->blocksize = blocksize;
  acache->numsets = (size/blocksize)/assoc;
  acache->BO = log2(blocksize);
  acache->TO = acache->BO + log2(acache->numsets);
  acache->VAImask = (1<< (unsigned long long)log2(acache->numsets)) - 1;

  acache->VATmask = ~(0);
}

// returns the set index, given an address
unsigned long long getindex(cache* acache, unsigned long long address){
    unsigned long long shifted = address >> acache->BO;
    return shifted & acache->VAImask; //Replace with correct value
}

// returns the tag, given an address
unsigned long long gettag(cache* acache, unsigned long long address){
    unsigned long long shifted = address >> acache->TO;
    return shifted; //Replace with correct value
}

// writes back the entire cache line at index, waynum into the next level of cache (e.g. L1->L2 or L2->Mem, etc.)
void writeback(cache* acache, unsigned int index, unsigned int oldestway){

  // YOUR CODE GOES HERE
  unsigned long long new_address = 0;
  unsigned long long new_tag = 0;
  const int to_byte = 8;
  unsigned long long temp_address;
  new_tag = acache->sets[index].blocks[oldestway].tag;
  new_address = (new_tag << acache->TO) | (index << acache->BO);

  int num_of_iters = acache->blocksize / to_byte;
  Bool write = 1;
  Bool blockaccess;
  for(int i = 0; i < num_of_iters; i++){
    temp_address = to_byte * i + new_address;
    blockaccess = i;
    perform_access(acache->nextcache, temp_address, to_byte, write, acache->sets[index].blocks[oldestway].datawords[i], blockaccess);
  }
}

// fills (i.e. reads) the entire cache line that contains address "address" from the next level of cache/memory and places the read values into the current level of cache at index, waynum
void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){

  // YOUR CODE GOES HERE
  unsigned long long value;
  // convert address to an integer address that is a multiple of blocksize
  unsigned long long base_addr = (address/acache->blocksize)*acache->blocksize;
  unsigned long long temp_address;
  const int to_byte = 8;
  int num_of_iters = acache->blocksize / to_byte;
  Bool write = 0;
  Bool blockaccess;
  unsigned long long val = 0;
  for(int i = 0; i < num_of_iters; i++){
    temp_address = to_byte * i + base_addr;
    blockaccess = i;
    value = perform_access(acache->nextcache, temp_address, to_byte, write, val, blockaccess);
    acache->sets[index].blocks[oldestway].datawords[i]= value;
  }
}
