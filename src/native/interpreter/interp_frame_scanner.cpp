// interp_frame_scanner.cpp — Precise GC scanning for interpreter frames
//
// Registered as a callback with the runtime-core GC scanner during interpreter
// initialization.  Walks the interpreter frame chain (FastFrame/RegisterFrame)
// and reports ObjectRef-typed stack/locals/register slots as precise GC roots.
//
// This file lives in the interpreter module so it can access the full
// FastFrame and RegisterFrame type definitions without creating a
// cross-module include dependency in runtime-core.

#include "fast_dispatch.h"
#include "ir_reg_alloc.h"
#include <gc_root_scanner.h>
#include <chaos/log.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// Use RegisterFrame from the interpreter namespace.
using interpreter::RegisterFrame;

// ── FastFrame scanner ──────────────────────────────────────────────

/// Scan a single FastFrame for ObjectRef roots in stack and locals.
static void GcScanFastFrame(const FastFrame& frame, GcRootCallback callback, void* user_data) noexcept {
    // Scan evaluation stack slots (0..sp-1).
    for (uint32_t i = 0; i < frame.sp; ++i) {
        if (frame.stack_tags[i] == static_cast<uint8_t>(interpreter::ValueTag::ObjectRef)) {
            if (frame.stack[i] != 0) {
                callback(const_cast<uint64_t*>(&frame.stack[i]),
                         /*is_interior=*/false, user_data);
            }
        }
    }

    // Scan local variable slots (0..local_count-1).
    for (uint32_t i = 0; i < frame.local_count; ++i) {
        if (frame.local_tags[i] == static_cast<uint8_t>(interpreter::ValueTag::ObjectRef)) {
            if (frame.locals[i] != 0) {
                callback(const_cast<uint64_t*>(&frame.locals[i]),
                         /*is_interior=*/false, user_data);
            }
        }
    }
}

// ── RegisterFrame scanner ──────────────────────────────────────────

/// Scan a single RegisterFrame for ObjectRef roots in the GPR file.
static void GcScanRegisterFrame(const RegisterFrame& frame, GcRootCallback callback, void* user_data) noexcept {
    const auto& regs = frame.regs;
    for (uint32_t i = 0; i < interpreter::kGPRegisters; ++i) {
        if (regs.gpr_tags[i] == static_cast<uint8_t>(interpreter::ValueTag::ObjectRef)) {
            if (regs.gpr[i] != 0) {
                callback(const_cast<uint64_t*>(&regs.gpr[i]),
                         /*is_interior=*/false, user_data);
            }
        }
    }
}

// ── Frame chain walker (registered callback) ───────────────────────

/// Walk the interpreter frame chain starting at @a root_frame and precisely
/// scan each frame for ObjectRef roots.  Handles both FastFrame and
/// RegisterFrame, identified by their frame_type discriminator at offset 0.
void GcScanInterpFrameChain(void* root_frame, GcRootCallback callback, void* user_data) noexcept {
    auto* current = static_cast<uint8_t*>(root_frame);

    while (current != nullptr) {
        auto frame_type = *reinterpret_cast<const uint32_t*>(current);

        if (frame_type == kInterpFrameType_FastFrame) {
            auto* ff = reinterpret_cast<FastFrame*>(current);
            GcScanFastFrame(*ff, callback, user_data);
            current = static_cast<uint8_t*>(ff->prev_frame);
        } else if (frame_type == kInterpFrameType_RegisterFrame) {
            auto* rf = reinterpret_cast<RegisterFrame*>(current);
            GcScanRegisterFrame(*rf, callback, user_data);
            current = static_cast<uint8_t*>(rf->prev_frame);
        } else {
            // Unknown frame type — stop chain walk to prevent
            // misinterpretation of arbitrary memory as frame pointers.
            CHAOS_IL2CPP_LOG_WARN_M("CRAG", "interp_frame_unknown_type: 0x%08x, stopping chain walk", frame_type);
            break;
        }
    }
}

// ── Module initialization ──────────────────────────────────────────

/// Register the interpreter frame scanner with the runtime-core GC system.
/// Called once during interpreter module startup.
void RegisterInterpFrameScanner() noexcept {
    GcSetInterpFrameScanner(GcScanInterpFrameChain);
}

} // namespace chaos::il2cpp::runtime_core
