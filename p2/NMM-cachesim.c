#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "NMM.h"
#include "csim.h"
#include "utils.h"

cache * closestcache=NULL;

int main (int argc, char** argv){
  init_struct initfields = init(argc, argv);
  uint64_t* allMatrices = initfields.allMatrices;
  unsigned int N = initfields.N;
  unsigned int iter = initfields.iterations;

  uint64_t*  A = allMatrices;
  uint64_t*  B = A + (N*N);
  uint64_t*  C = B + (N*N);
  uint64_t*  D = C + (N*N);

  for(unsigned int itercount=0;itercount<iter;++itercount){
    // rotate ABC matrix order for each iteration
    A += (N*N); A=(A==(allMatrices + (4*N*N)) )?allMatrices:A; 
    B += (N*N); B=(B==(allMatrices + (4*N*N)) )?allMatrices:B; 
    C += (N*N); C=(C==(allMatrices + (4*N*N)) )?allMatrices:C;
    D += (N*N); D=(D==(allMatrices + (4*N*N)) )?allMatrices:D;
    // perform C = AB Naive Matrix Multiplication through simulated cache
    NaiveDDPMatMul(N, C, A, B);
    NaiveDDPMatTranspose(N, C, D);
  }

  printsquarematrix(N, C); // Just print the in-memory contents of the last C matrix
  printstatistics(closestcache); // Print all cache statistics
  printcachecontents(closestcache); // Print all cache contents and metadata

  return 0;
}

// handle command line inputs, intiialize data structures
init_struct init (int argc, char** argv){
  init_struct the_struct;

  if(argc<8 || argc%4!=0){
    fprintf(stderr,"Wrong number of arguments\nTakes DEBUG_FLAG N(Matrix Dimensions) Iter(Number of iterations) Memory 4-tuples<name,size,assoc,blocksize>\n");
    fprintf(stderr,"Example:\n");
    fprintf(stderr,"\t./NMM-csim.bin 1 2 1 MainMem 1 1 1\n");
    fprintf(stderr,"\t./NMM-csim.bin 1 2 23 MainMem 1 1 1 L2 128 1 32 L1 64 1 32\n");
    exit(-1);
  }

  the_struct.DEBUG = atoi(argv[1]);
  the_struct.N = atoi(argv[2]); // Size of NxN matrices A, B, C
  the_struct.iterations = atoi(argv[3]); // Number of matrix multiplications to perform
  the_struct.allMatrices=(uint64_t*)valloc(4*the_struct.N*the_struct.N*sizeof(uint64_t)); //Allocate memory for A, B, C

  if(!(the_struct.allMatrices)){
    fprintf(stderr,"memory allocation failure\n");
    exit(-1);
  }

  // Fill allocated memory with deterministic pattern
  patternFill(the_struct.allMatrices,4*the_struct.N*the_struct.N);

  unsigned int memlevel=0;
  unsigned int nextarg=4;
  while(nextarg<argc){
    // extract cache params for each level
    char * curname = argv[nextarg++];
    unsigned int cursize = atoi(argv[nextarg++]);
    unsigned int curassoc = atoi(argv[nextarg++]);
    unsigned int curblocksize = atoi(argv[nextarg++]);
    if (memlevel > 0 && (curblocksize < 8 || curblocksize%8) ){
      fprintf(stderr, "cache block size is < one 64-bit word or not a multiple of 64-bit words, exiting.");
      exit(-1);
    } 
    // generate cache with extracted params
    cache * curcache=initcache(cursize, curassoc, curblocksize, curname, !(memlevel++));
    curcache->DEBUG = the_struct.DEBUG;
    // connect to cache hierarchy
    linkcache(curcache,closestcache);
    closestcache=curcache;
  }
  
  return the_struct;
} 

// C = A*B for NxN matrices A, B, C - performed through simulated cache system
void NaiveDDPMatMul(unsigned int N, uint64_t* C, uint64_t* A, uint64_t* B){
  unsigned int i,j,k;
  uint64_t ipsum;
  for(i=0;i<N;++i){
    for(j=0;j<N;++j){
      ipsum=0; // init C[i,j]
      for(k=0;k<N;++k){
        // annotation: read A[i,k] from cache
        // annotation: read B[k,j] from cache 
	ipsum += LoadWord(closestcache, (uint64_t)(&(A[i*N+k])) ) * LoadWord(closestcache, (uint64_t)(&(B[k*N+j])) );
      }
      // annotation: write C[i,j]=ipsum to cache
      StoreWord(closestcache, (uint64_t)(&(C[i*N+j])), ipsum);
    }
  }
}

// A = A^T for NxN matrice C - performed through simulated cache system
void NaiveDDPMatTranspose(unsigned int N, uint64_t* A, uint64_t* temp){
  unsigned int i,j,k;
// Create transpose of A in temp
  for(i=0;i<N;++i){
    for(j=0;j<N;++j){
      uint64_t x = LoadWord(closestcache, (uint64_t)(&(A[i*N+j])));
      StoreWord(closestcache, (uint64_t)(&(temp[j*N+i])), x);
    }
  }
// Copy temp to A
  for(i=0;i<N;++i){
    for(j=0;j<N;++j){
      uint64_t x = LoadWord(closestcache, (uint64_t)(&(temp[i*N+j])));
      StoreWord(closestcache, (uint64_t)(&(A[i*N+j])), x);
    }
  }
}

