#include <cstdio>
extern "C" int RunNativeAot(int);
int main() { int x=RunNativeAot(0); printf("%d
",x); return 0; }
