// fast_dispatch.cpp — Unity-build aggregator
//
// This file aggregates all sub-files into a single translation unit.
// Edit individual .inc files or regenerate from fast_dispatch_full.cpp.bak
// using the split_fast_dispatch.py script.
//
// The preamble (includes + namespace opens) lives in fast_dispatch_core.inc.
// All other .inc files are included within namespace chaos::il2cpp::runtime_core.

#include "fast_dispatch_core.inc"
#include "fast_dispatch_branch.inc"
#include "fast_dispatch_arithmetic.inc"
#include "fast_dispatch_call.inc"
#include "fast_dispatch_seh.inc"
#include "fast_dispatch_object.inc"
#include "fast_dispatch_type.inc"
#include "fast_dispatch_execute.inc"
} // namespace chaos::il2cpp::runtime_core
