#include <cstdio>
#include <csetjmp>
extern "C" void SetExceptionFallback(void(*));
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf; static void fb() { longjmp(buf, 1); }
int main() { printf("t4\n"); fflush(stdout); SetExceptionFallback(fb); if (setjmp(buf) == 0) { RunNativeAot(0); } else { printf("threw\n"); } return 0; }
