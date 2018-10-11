__kernel void reduce_sum(global int* restrict in, const int sz, global int* restrict result) {
  printf("Hello from Altera's OpenCL Compiler!\n");

  int shreg[1024];
  #pragma unroll
  for(int i=0; i<sz; i++) {
  	shreg[i]=0;
  }
  int sum=0;
  for (int j=0; j<sz; j++){
  	#pragma unroll
  	for(int i=0; i<(sz-1); i++) {
  		shreg[i]=shreg[i+1];
  	}
  	shreg[sz-1]=in[j];
  
  	sum+=shreg[sz-1];
  }
  *result=sum;
}

