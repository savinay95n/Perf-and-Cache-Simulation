#include "YOURCODEHERE.h"

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

}

// returns the set index, given an address
unsigned long long getindex(cache* acache, unsigned long long address){
  return 0; //Replace with correct value
}

// returns the tag, given an address
unsigned long long gettag(cache* acache, unsigned long long address){
  return 0; //Replace with correct value
}

// writes back the entire cache line at index, waynum into the next level of cache (e.g. L1->L2 or L2->Mem, etc.)
void writeback(cache* acache, unsigned int index, unsigned int oldestway){

  // YOUR CODE GOES HERE

}

// fills (i.e. reads) the entire cache line that contains address "address" from the next level of cache/memory and places the read values into the current level of cache at index, waynum
void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){

  // YOUR CODE GOES HERE

}
