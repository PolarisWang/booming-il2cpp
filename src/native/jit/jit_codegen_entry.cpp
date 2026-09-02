// jit_codegen_entry.cpp — Compile/CanCompile entry points and JitMethod
// lifecycle (destructor, move ctor/assign) for NativeCodeGenerator
// (T2.4 module split of jit_engine.cpp).
//
// Pure move: these were relocated verbatim from jit_codegen_generate.cpp;
// no logic changed.

#ifndef NOMINMAX
#define NOMINMAX 1  // avoid Windows min/max macro clashing with std::min/std::max
#endif
#include "jit_engine.h"
#include "ArchTraits.h"
#include "jit_helpers.h"
#include "jit_seh.h"    // RegisterNativeCodeSection
#include "jit_unwind.h" // AllocRuntimeFunction, EmitDwarfCie, EmitDwarfFde
#include "slot_map.h"
#include "tree/jit_optimizer.h"
#include "code_buffer.h"
#include "IEncoder.h"
#include "codegen_bridge.h" // GcPointMapV0, GcSafepointV0, CHAOS_GC_SLOT_*
#include "jit_inline.h"     // g_inline_reverse_map
#include "../interpreter/ir_reg_alloc.h"
#include <gc_root_scanner.h> // GcRegisterSlotMap
#include <instantiation_engine.h>
#include <chaos/profile.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // PRUNTIME_FUNCTION (Win64 .pdata/.xdata)
#endif
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "x64_encoder.h"
#include "X64Encoder.h"
#endif

#include <algorithm> // std::min/std::max

namespace chaos::il2cpp::jit {

JitMethod* Compile(const ::chaos::il2cpp::interpreter::RegisterMethod& rm, const CompileConfig& config) noexcept {
    if (rm.instructions.empty())
        return nullptr;
    ISehHandler& seh = GetSehHandler();
    NativeCodeGenerator gen(rm, config, seh);
    return gen.Generate();
}

bool CanCompile(const ::chaos::il2cpp::interpreter::RegisterMethod& rm) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (rm.instructions.empty())
        return false;
    // Validate SEH clause indices — any out-of-range clause means the
    // IR is malformed.  Generate() skips them, but early rejection here
    // prevents partial codegen.
    uint32_t n_instrs = static_cast<uint32_t>(rm.instructions.size());
    for (const auto& clause : rm.seh_clauses) {
        if (static_cast<uint32_t>(clause.try_start_idx) >= n_instrs ||
            static_cast<uint32_t>(clause.try_end_idx) > n_instrs ||
            static_cast<uint32_t>(clause.handler_start_idx) >= n_instrs) {
            return false;
        }
    }
    return true; // All opcodes accepted — unsupported ones deopt at runtime.
}

JitMethod::~JitMethod() noexcept {
    CHAOS_IL2CPP_FREE(call_sites);

    // Remove this method's slots from the reverse map (so hotpatch updates
    // don't try to write to freed JitMethod pointers).
    g_reverse_slot_map.RemoveAll(this);
    CHAOS_IL2CPP_FREE(deopt_entries);
    CHAOS_IL2CPP_FREE(deopt_values);
    CHAOS_IL2CPP_FREE(gc_points);
    // Unregister GC slot map before freeing the backing data.
    if (code != nullptr && gc_slot_map != nullptr) {
        chaos::il2cpp::runtime_core::GcUnregisterSlotMap(code);
    }
    CHAOS_IL2CPP_FREE(slot_map_data);
    CHAOS_IL2CPP_FREE(gc_point_map_data);
    CHAOS_IL2CPP_FREE(instr_offsets);
    // Free GcPoint.slots arrays (each allocated independently by RecordGcPoint)
    for (uint32_t i = 0; i < gc_point_count; ++i) {
        CHAOS_IL2CPP_FREE(gc_points[i].slots);
    }
#if defined(_WIN64)
    if (runtime_function != nullptr) {
        RtlDeleteFunctionTable(static_cast<PRUNTIME_FUNCTION>(runtime_function));
        CHAOS_IL2CPP_FREE(runtime_function);
        runtime_function = nullptr;
    }
#elif defined(__linux__)
    if (eh_frame_registered && code != nullptr) {
        const void* eh_frame = static_cast<const uint8_t*>(code) + eh_frame_offset;
        __deregister_frame(eh_frame);
    }
#endif
    if (code != nullptr && !code_managed_externally) {
        chaos::il2cpp::pal::PalVirtualFree(code, code_size);
    }
    code = nullptr;
}

JitMethod::JitMethod(JitMethod&& other) noexcept {
    std::memcpy(this, &other, sizeof(*this));
    std::memset(&other, 0, sizeof(other));
}

JitMethod& JitMethod::operator=(JitMethod&& other) noexcept {
    if (this != &other) {
        this->~JitMethod();
        std::memcpy(this, &other, sizeof(*this));
        std::memset(&other, 0, sizeof(other));
    }
    return *this;
}

} // namespace chaos::il2cpp::jit
