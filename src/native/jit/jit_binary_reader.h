// jit_binary_reader.h — Standalone Binary IR serialization/deserialization for single-method RegisterMethod
//
// Defines the v1 Binary IR format for transporting a single RegisterMethod
// between codegen output and the JIT runtime.  Used by the JIT and Hybrid
// codegen modes to embed pre-allocated register IR in the generated output.
//
// Format (all integers little-endian):
//   [BinaryIrHeader]          (32 bytes)
//   [RegisterInstruction[]]   (instr_count × 16 bytes)
//   [RegisterSehClause[]]     (seh_count × 24 bytes, from register_ir.h)
//   [BinaryCatchHandlerEntry] (catch_handler_count × 12 bytes)
//   [uint32_t[]]              (il_offset_count × 4 bytes)
//   [BinaryStackMapEntry]     (stack_map_count × 25 bytes)

#ifndef CHAOS_IL2CPP_JIT_BINARY_READER_H_
#define CHAOS_IL2CPP_JIT_BINARY_READER_H_

#include "interpreter_vm.h"  // SEHClause, CatchHandlerEntry
#include "ir_reg_alloc.h"    // RegisterMethod, RegisterInstruction, RegStackMap, RegStackMapEntry
#include <register_ir.h>     // RegisterSehClause (C-compatible SEH clause)

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::jit {

// ── Magic number ────────────────────────────────────────────────────────
// "BIR\0" = 0x00524942 (little-endian)
static constexpr uint32_t kBinaryIrMagic = 0x00524942u;
static constexpr uint32_t kBinaryIrVersion = 1u;

// ── CatchHandlerEntry compact (12 bytes) ────────────────────────────────
#pragma pack(push, 1)
struct BinaryCatchHandlerEntry {
    uint32_t handler_start_idx;
    uint8_t  exception_reg;
    uint8_t  _pad[3];     // padding to align class_token
    uint32_t class_token;
};
#pragma pack(pop)
static_assert(sizeof(BinaryCatchHandlerEntry) == 12,
    "BinaryCatchHandlerEntry must be 12 bytes");

// ── StackMapEntry compact (25 bytes) ────────────────────────────────────
#pragma pack(push, 1)
struct BinaryStackMapEntry {
    int8_t   slot_regs[16];
    int8_t   local_regs[8];
    uint8_t  stack_depth;
};
#pragma pack(pop)
static_assert(sizeof(BinaryStackMapEntry) == 25,
    "BinaryStackMapEntry must be 25 bytes");

// ── Binary IR header (32 bytes, packed) ─────────────────────────────────
// Uses uint16_t for bounded fields to keep total size at 32 bytes.
#pragma pack(push, 1)
struct BinaryIrHeader {
    uint32_t magic;                 // kBinaryIrMagic      [0..4)
    uint32_t version;               // kBinaryIrVersion    [4..8)
    uint32_t max_regs;              // highest register    [8..12)
    uint16_t instr_count;           // instr count         [12..14)
    uint16_t seh_count;             // SEH clause count    [14..16)
    uint16_t catch_handler_count;   // catch handler count [16..18)
    uint16_t il_offset_count;       // IL offset count     [18..20)
    uint16_t stack_map_count;       // stack map count     [20..22)
    uint16_t _reserved;             // padding             [22..24)
    uint32_t total_size;            // total data bytes    [24..28)
    uint32_t _reserved2;            // future expansion    [28..32)
};
#pragma pack(pop)
static_assert(sizeof(BinaryIrHeader) == 32,
    "BinaryIrHeader must be 32 bytes");

// ── Data size helper ───────────────────────────────────────────────────
inline uint32_t BinaryIrDataSize(const BinaryIrHeader& hdr) noexcept {
    return static_cast<uint32_t>(hdr.instr_count)         * sizeof(interpreter::RegisterInstruction) +
           static_cast<uint32_t>(hdr.seh_count)           * sizeof(RegisterSehClause) +
           static_cast<uint32_t>(hdr.catch_handler_count) * sizeof(BinaryCatchHandlerEntry) +
           static_cast<uint32_t>(hdr.il_offset_count)     * sizeof(uint32_t) +
           static_cast<uint32_t>(hdr.stack_map_count)     * sizeof(BinaryStackMapEntry);
}

// ── Accessor helpers ───────────────────────────────────────────────────
inline uint32_t BinaryIrInstrCount(const BinaryIrHeader& hdr) noexcept { return hdr.instr_count; }
inline uint32_t BinaryIrSehCount(const BinaryIrHeader& hdr) noexcept { return hdr.seh_count; }
inline uint32_t BinaryIrCatchHandlerCount(const BinaryIrHeader& hdr) noexcept { return hdr.catch_handler_count; }
inline uint32_t BinaryIrIlOffsetCount(const BinaryIrHeader& hdr) noexcept { return hdr.il_offset_count; }
inline uint32_t BinaryIrStackMapCount(const BinaryIrHeader& hdr) noexcept { return hdr.stack_map_count; }

// ── Serialize RegisterMethod → binary buffer ───────────────────────────
// Returns a heap-allocated buffer containing the standalone Binary IR.
// The caller owns the buffer and must free it with std::free().
// Returns nullptr on failure (empty method, allocation error).
uint8_t* SerializeBinaryIr(
    const interpreter::RegisterMethod& rm,
    size_t* out_size) noexcept;

// ── Deserialize binary buffer → RegisterMethod ─────────────────────────
// Reads the Binary IR from a validated buffer and reconstructs a
// RegisterMethod.  The returned method owns heap copies of all data
// (it does NOT alias the input buffer).
// Returns an empty RegisterMethod (instructions.empty()) on failure.
interpreter::RegisterMethod DeserializeBinaryIr(
    const uint8_t* data, size_t size) noexcept;

// ── Validate binary buffer ────────────────────────────────────────────
// Checks magic, version, total_size consistency.
// Returns true if the buffer appears to be valid v1 Binary IR.
bool ValidateBinaryIr(const uint8_t* data, size_t size) noexcept;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_JIT_BINARY_READER_H_
