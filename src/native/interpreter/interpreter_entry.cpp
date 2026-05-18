// ABI exports: extern "C" linkage for managed/NativeAOT callability.
//
// Unity-file aggregator: interpreter_entry.cpp is the single translation unit.
// Each interp_entry/*.cpp file is #included here and contributes to one combined
// compilation unit. Zero CMakeLists.txt changes required.

#include "interpreter_entry.h"
#include "patch_loader.h"
#include "runtime_core.h"
#include "instantiation_engine.h"
#include "token_resolver.h"
#include "module_registry.h"
#include "reflection_query_model.h"
#include "fast_frame_pool.h"
#include "fast_dispatch.h"

#include "bootstrap/bootstrap.h"

#include <aot_core_ir_reader.h>   // DeserializeAotCoreIrMethod
#include <interpreter_vm.h>       // ExecutionFrame, InterpreterVM, IRMethod, InterpreterValue

#include <ir_reg_alloc.h>         // AllocateRegisters, RegisterExecute, RegisterFrame

#include <atomic>
#include <cstring>
#include <mutex>
#include <chaos/log.h>
#include <chaos/profile.h>


#include "interp_entry/arg_buffer.cpp"
#include "interp_entry/subject_id_resolver.cpp"
#include "interp_entry/aot_direct_dispatch.cpp"
#include "interp_entry/hotpatch_resolve.cpp"
#include "interp_entry/signature_cache.cpp"
#include "interp_entry/inlining.cpp"
#include "interp_entry/patch_method_lower.cpp"
#include "interp_entry/entry_direct.cpp"
#include "interp_entry/reapply_inlining.cpp"
