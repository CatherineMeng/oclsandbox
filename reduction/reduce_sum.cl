__kernel void reduce_sum(global int* restrict in, const int sz, global int* restrict result) {
  printf("Hello from Altera's OpenCL Compiler!\n");

  int shreg[2];
  #pragma unroll
  for(int i=0; i<2; i++) {
  	shreg[i]=0;
  }
  int sum=0;
  for (int j=0; j<sz; j++){
  	shreg[0]=shreg[1];
	shreg[1]=in[j];
  
  	sum+=shreg[1];
  }
  *result=sum;
}

