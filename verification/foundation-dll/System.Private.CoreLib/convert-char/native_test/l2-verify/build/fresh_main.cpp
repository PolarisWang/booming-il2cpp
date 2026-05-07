#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void(*fn)());
extern "C" int RunNativeAot(int);
#include "expected_checksums.h"
static jmp_buf buf; static void fb(){longjmp(buf,1);}
int main(){
__chaos_assert_failures=0; SetExceptionFallback(fb); int rf=0;
for(int i=0;i<kExpectedCount;i++){
int a=0;bool t=false;if(setjmp(buf)==0){a=RunNativeAot(i);}else{t=true;}
if(kExpectedChecksums[i]==-1)continue;
if(t){printf("FAIL [%d]: threw, exp %d\n",i,kExpectedChecksums[i]);rf++;continue;}
if(a!=kExpectedChecksums[i]){printf("FAIL [%d]: exp %d got %d\n",i,kExpectedChecksums[i],a);rf++;}
}
SetExceptionFallback(nullptr);
printf("L2: %d/%d passed\n",kExpectedCount-rf,kExpectedCount);
return __chaos_assert_failures+rf;
}
