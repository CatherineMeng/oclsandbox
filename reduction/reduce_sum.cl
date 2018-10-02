__kernel void reduce_sum(global int* restrict in, const int sz, global int* restrict result) {
  printf("Hello from Altera's OpenCL Compiler!\n");
  *result = 0;
  for(int i=0; i<sz; i++) {
    *result += in[i];
  }
}

