#include <cstdio>
extern "C" void SetExceptionFallback(void(*));
int main() { printf("t1\n"); fflush(stdout); SetExceptionFallback(nullptr); return 0; }
