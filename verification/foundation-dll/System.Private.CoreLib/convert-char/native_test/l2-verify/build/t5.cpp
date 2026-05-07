#include <cstdio>
#include <csetjmp>
extern "C" void SetExceptionFallback(void(*));
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf; static void fb() { longjmp(buf, 1); }
int main() { SetExceptionFallback(fb); for (int i=0; ikExpectedCount; i++) { if (setjmp(buf)==0) { int a = RunNativeAot(i); printf("[d\n", i, a); } else { printf("[d] threw\n", i); } } fflush(stdout); return 0; }
