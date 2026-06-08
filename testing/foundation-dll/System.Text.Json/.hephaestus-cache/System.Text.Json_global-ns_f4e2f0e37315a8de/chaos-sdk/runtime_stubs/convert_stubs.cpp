// convert_stubs.cpp — String-to-number and number-to-string stubs
//
// NOTE: This file intentionally left empty. All ChaosConvert* / ChaosFormat*
// / ChaosParse* / ChaosDecimal* functions are implemented in parse_convert.cpp
// (which lives one directory up and uses the correct CHAOS_IL2CPP_STRING_TYPE
// layout). Keeping a stub version creates duplicate symbol conflicts (LNK4006)
// where the linker picks the stub over the real implementation.
//
// The file is retained so the build system doesn't break, but no function
// definitions live here.
#include "generated_code_compat.h"