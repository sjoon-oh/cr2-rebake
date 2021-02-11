#define cudaErrorCheck(func) { checkError((func), __FILE__, __LINE__); }
inline
void checkError(cudaError_t func, const char* file, int line) {
  if (func!= cudaSuccess) {
    fprintf(stderr, "GPU error: %s %s %d\n", 
            cudaGetErrorString(func), file, line);
    exit(-1);
  }
}
