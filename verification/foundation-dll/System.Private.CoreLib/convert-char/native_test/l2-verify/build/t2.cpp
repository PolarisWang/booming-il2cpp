#include <cstdio>
extern "C" void SetExceptionFallback(void(*));
extern "C" int RunNativeAot(int);
int main() { printf("t2\n"); fflush(stdout); SetExceptionFallback(nullptr); return 0; }
