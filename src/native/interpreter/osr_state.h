#ifndef CHAOS_IL2CPP_OSR_STATE_H_
#define CHAOS_IL2CPP_OSR_STATE_H_

// ── OsrState: canonical frame state for on-stack replacement ──────────────
//
// Captures the minimal execution state needed to transfer a method between
// tiers.  Every converter (Capture/Restore) translates between OsrState and
// a tier-specific frame format.
//
// Memory ownership: when an OsrState holds tracked objects, ownership IS
// transferred — the OsrState destructor calls cleanup.  The source frame
// MUST NOT also clean up (skip CleanupTracked / ~ExecutionFrame).

#include <cstdint>
#include <cstring>
#include <chaos/native_types.h>   // CHAOS_IL2CPP_FREE (Cleanup releases heap tracked objects)

namespace chaos::il2cpp::runtime_core {
struct FastFrame;
}

namespace chaos::il2cpp::interpreter {

struct ExecutionFrame;
struct ExecutionResult;
class InterpreterVM;
struct RegisterFrame;
struct RegStackMapEntry;

struct OsrState {
    static constexpr uint32_t kMaxStack = 64;
    static constexpr uint32_t kMaxLocals = 32;
    static constexpr uint32_t kMaxTracked = 32;

    uint32_t pc = 0;
    uint32_t sp = 0;
    uint64_t stack[kMaxStack] = {};
    uint8_t stack_tags[kMaxStack] = {};
    uint64_t locals[kMaxLocals] = {};
    uint8_t local_tags[kMaxLocals] = {};
    uint32_t local_count = 0;

    // Tracked interpreter-heap objects (InterpreterObject / ArrayStorage).
    void* tracked_objs[kMaxTracked] {};
    void (*tracked_dtors[kMaxTracked])(void*) {};
    bool tracked_is_pool[kMaxTracked] {};   // true → dtor returns to pool, skip FREE
    uint32_t tracked_cnt = 0;

    ~OsrState() noexcept { Cleanup(); }

    // No copying — ownership of tracked objects is exclusive.
    OsrState(const OsrState&) = delete;
    OsrState& operator=(const OsrState&) = delete;

    // Move: transfer tracked object ownership.
    OsrState(OsrState&& other) noexcept { TransferFrom(other); }
    OsrState& operator=(OsrState&& other) noexcept {
        if (this != &other) {
            Cleanup();
            TransferFrom(other);
        }
        return *this;
    }

    OsrState() = default;

    void Cleanup() noexcept {
        for (uint32_t i = 0; i < tracked_cnt; ++i) {
            tracked_dtors[i](tracked_objs[i]);
            // Pool objects' dtor (e.g. ReturnBoxToPool) already returns them to
            // their caller pool — do NOT CHAOS_IL2CPP_FREE those.  Heap objects
            // whose dtor is a plain destructor still need an explicit FREE.
            if (!tracked_is_pool[i]) {
                CHAOS_IL2CPP_FREE(tracked_objs[i]);
            }
        }
        tracked_cnt = 0;
    }

private:
    void TransferFrom(OsrState& other) noexcept {
        pc = other.pc;
        sp = other.sp;
        local_count = other.local_count;
        tracked_cnt = other.tracked_cnt;

        std::memcpy(stack, other.stack, sizeof(stack));
        std::memcpy(stack_tags, other.stack_tags, sizeof(stack_tags));
        std::memcpy(locals, other.locals, sizeof(locals));
        std::memcpy(local_tags, other.local_tags, sizeof(local_tags));
        std::memcpy(tracked_objs, other.tracked_objs, sizeof(tracked_objs));
        std::memcpy(tracked_dtors, other.tracked_dtors, sizeof(tracked_dtors));
        std::memcpy(tracked_is_pool, other.tracked_is_pool, sizeof(tracked_is_pool));

        other.tracked_cnt = 0; // Ownership transferred — don't double-free.
    }
};

} // namespace chaos::il2cpp::interpreter

// ── OsrState converter functions ──────────────────────────────────────────

namespace chaos::il2cpp::interpreter {

/// Capture VM execution state into OsrState.
/// Sets result.wants_osr = true and result.osr_state when conversion succeeds.
void CaptureVmState(ExecutionResult& result, const ExecutionFrame& frame, uint32_t pc, uint32_t local_count) noexcept;

/// Restore OsrState into a FastFrame (for OSR promotion: VM → FastExecute).
/// Does NOT modify tracked_objs — sets frame fields directly.
void RestoreOsrToFastFrame(const OsrState& osr, runtime_core::FastFrame& frame) noexcept;

/// Capture FastFrame execution state into OsrState.
/// Transfers tracked object ownership from frame to osr (skips cleanup in frame).
void CaptureFastFrame(OsrState& osr, const runtime_core::FastFrame& frame) noexcept;

/// Restore OsrState into an ExecutionFrame (for OSR demotion: FastExecute → VM).
void RestoreOsrToVmFrame(const OsrState& osr, ExecutionFrame& frame) noexcept;

/// Capture RegisterFrame state into OsrState (for OSR demotion: RegisterExecute → FastExecute/VM).
/// Uses RegStackMapEntry to map register file positions back to evaluation stack slots.
void CaptureRegisterFrame(OsrState& osr, const RegisterFrame& frame, const RegStackMapEntry& stack_entry,
                          uint32_t arg_count, uint32_t local_count) noexcept;

/// Restore OsrState into a RegisterFrame (for OSR promotion: FastExecute/VM → RegisterExecute).
/// Uses RegStackMapEntry to map evaluation stack slots to virtual registers.
void RestoreOsrToRegisterFrame(const OsrState& osr, RegisterFrame& frame, const RegStackMapEntry& stack_entry,
                               uint32_t arg_count, uint32_t local_count) noexcept;

/// Capture native code frame state into OsrState (for deoptimization: native → interpreter).
/// The native code frame stores all virtual registers in a fixed stack layout
/// (see code_generator.cpp frame layout constants).  This converter reads from
/// the flat register file snapshot and reconstructs the evaluation stack and
/// locals using the RegStackMapEntry at the captured pc.
///
/// @param osr           Output: captured OSR state.
/// @param gpr_file      GPR register file snapshot from the native frame[64].
/// @param fpr_file      FPR register file snapshot from the native frame[32].
/// @param stack_entry   RegStackMapEntry for the captured pc.
/// @param arg_count     Number of method arguments.
/// @param local_count   Number of local variables (max OsrState::kMaxLocals).
void CaptureNativeFrame(OsrState& osr, const uint64_t* gpr_file, const double* fpr_file,
                        const RegStackMapEntry& stack_entry, uint32_t arg_count, uint32_t local_count,
                        const uint8_t* gpr_tags = nullptr) noexcept;

} // namespace chaos::il2cpp::interpreter

#endif // CHAOS_IL2CPP_OSR_STATE_H_
