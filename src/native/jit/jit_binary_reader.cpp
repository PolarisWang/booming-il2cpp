// jit_binary_reader.cpp — Standalone Binary IR serialization/deserialization
//
// Implements the v1 Binary IR format for single-method RegisterMethod transport
// between codegen output and the JIT runtime.

#include "jit_binary_reader.h"

#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::jit {

// ── Serialize RegisterMethod → binary buffer ───────────────────────────

uint8_t* SerializeBinaryIr(const interpreter::RegisterMethod& rm, size_t* out_size) noexcept {
    if (rm.instructions.empty()) {
        if (out_size)
            *out_size = 0;
        return nullptr;
    }

    uint32_t instr_count = static_cast<uint32_t>(rm.instructions.size());
    uint32_t seh_count = static_cast<uint32_t>(rm.seh_clauses.size());
    uint32_t catch_count = static_cast<uint32_t>(rm.catch_handler_entries.size());
    uint32_t il_count = static_cast<uint32_t>(rm.il_offsets.size());
    uint32_t sm_count = static_cast<uint32_t>(rm.stack_map.entries.size());

    // Bounds check for uint16_t fields
    if (instr_count > 0xFFFF || seh_count > 0xFFFF || catch_count > 0xFFFF || il_count > 0xFFFF || sm_count > 0xFFFF) {
        if (out_size)
            *out_size = 0;
        return nullptr;
    }

    // Total data size (after header)
    uint32_t data_sz = instr_count * sizeof(interpreter::RegisterInstruction) + seh_count * sizeof(RegisterSehClause) +
                       catch_count * sizeof(BinaryCatchHandlerEntry) + il_count * sizeof(uint32_t) +
                       sm_count * sizeof(BinaryStackMapEntry);

    uint32_t total_sz = sizeof(BinaryIrHeader) + data_sz;
    auto* buf = static_cast<uint8_t*>(std::malloc(total_sz));
    if (!buf) {
        if (out_size)
            *out_size = 0;
        return nullptr;
    }

    // Write header
    BinaryIrHeader hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.magic = kBinaryIrMagic;
    hdr.version = kBinaryIrVersion;
    hdr.max_regs = rm.max_regs;
    hdr.instr_count = static_cast<uint16_t>(instr_count);
    hdr.seh_count = static_cast<uint16_t>(seh_count);
    hdr.catch_handler_count = static_cast<uint16_t>(catch_count);
    hdr.il_offset_count = static_cast<uint16_t>(il_count);
    hdr.stack_map_count = static_cast<uint16_t>(sm_count);
    hdr._reserved = 0;
    hdr.total_size = data_sz;
    hdr._reserved2 = 0;
    std::memcpy(buf, &hdr, sizeof(hdr));

    size_t offset = sizeof(BinaryIrHeader);

    // Write RegisterInstruction array
    if (instr_count > 0) {
        size_t bytes = instr_count * sizeof(interpreter::RegisterInstruction);
        std::memcpy(buf + offset, rm.instructions.data(), bytes);
        offset += bytes;
    }

    // Write SEH clauses (convert SEHClause → RegisterSehClause)
    for (uint32_t i = 0; i < seh_count; ++i) {
        RegisterSehClause sc;
        sc.flags = static_cast<uint32_t>(rm.seh_clauses[i].flags);
        sc.try_start_idx = static_cast<uint32_t>(rm.seh_clauses[i].try_start_idx);
        sc.try_end_idx = static_cast<uint32_t>(rm.seh_clauses[i].try_end_idx);
        sc.handler_start_idx = static_cast<uint32_t>(rm.seh_clauses[i].handler_start_idx);
        sc.handler_end_idx = static_cast<uint32_t>(rm.seh_clauses[i].handler_end_idx);
        sc.class_token = rm.seh_clauses[i].class_token;
        std::memcpy(buf + offset, &sc, sizeof(sc));
        offset += sizeof(RegisterSehClause);
    }

    // Write CatchHandlerEntry array
    for (uint32_t i = 0; i < catch_count; ++i) {
        BinaryCatchHandlerEntry ce;
        ce.handler_start_idx = rm.catch_handler_entries[i].handler_start_idx;
        ce.exception_reg = rm.catch_handler_entries[i].exception_reg;
        ce._pad[0] = ce._pad[1] = ce._pad[2] = 0;
        ce.class_token = rm.catch_handler_entries[i].class_token;
        std::memcpy(buf + offset, &ce, sizeof(ce));
        offset += sizeof(BinaryCatchHandlerEntry);
    }

    // Write IL offsets
    if (il_count > 0) {
        size_t bytes = il_count * sizeof(uint32_t);
        std::memcpy(buf + offset, rm.il_offsets.data(), bytes);
        offset += bytes;
    }

    // Write stack map entries
    for (uint32_t i = 0; i < sm_count; ++i) {
        BinaryStackMapEntry se;
        std::memcpy(se.slot_regs, rm.stack_map.entries[i].slot_regs, sizeof(se.slot_regs));
        std::memcpy(se.local_regs, rm.stack_map.entries[i].local_regs, sizeof(se.local_regs));
        se.stack_depth = rm.stack_map.entries[i].stack_depth;
        std::memcpy(buf + offset, &se, sizeof(se));
        offset += sizeof(BinaryStackMapEntry);
    }

    if (out_size)
        *out_size = total_sz;
    return buf;
}

// ── Validate binary buffer ────────────────────────────────────────────

bool ValidateBinaryIr(const uint8_t* data, size_t size) noexcept {
    if (!data || size < sizeof(BinaryIrHeader))
        return false;

    const auto& hdr = *reinterpret_cast<const BinaryIrHeader*>(data);
    if (hdr.magic != kBinaryIrMagic)
        return false;
    if (hdr.version != kBinaryIrVersion)
        return false;

    // Sanity bounds
    if (hdr.instr_count == 0 || hdr.instr_count > 100000 || hdr.seh_count > 1000 || hdr.catch_handler_count > 1000 ||
        hdr.il_offset_count > 100000 || hdr.stack_map_count > 100000) {
        return false;
    }

    uint32_t expected_data = BinaryIrDataSize(hdr);
    if (hdr.total_size != expected_data)
        return false;

    size_t expected_total = sizeof(BinaryIrHeader) + hdr.total_size;
    if (size < expected_total)
        return false;

    return true;
}

// ── Deserialize binary buffer → RegisterMethod ─────────────────────────

interpreter::RegisterMethod DeserializeBinaryIr(const uint8_t* data, size_t size) noexcept {
    interpreter::RegisterMethod rm;

    if (!ValidateBinaryIr(data, size))
        return rm;

    const auto& hdr = *reinterpret_cast<const BinaryIrHeader*>(data);
    rm.max_regs = hdr.max_regs;

    uint32_t instr_count = hdr.instr_count;
    uint32_t seh_count = hdr.seh_count;
    uint32_t catch_count = hdr.catch_handler_count;
    uint32_t il_count = hdr.il_offset_count;
    uint32_t sm_count = hdr.stack_map_count;

    size_t offset = sizeof(BinaryIrHeader);

    // Read RegisterInstruction array
    if (instr_count > 0) {
        size_t bytes = instr_count * sizeof(interpreter::RegisterInstruction);
        rm.instructions.resize(instr_count);
        std::memcpy(rm.instructions.data(), data + offset, bytes);
        offset += bytes;
    }

    // Read SEH clauses (convert RegisterSehClause → SEHClause)
    rm.seh_clauses.resize(seh_count);
    for (uint32_t i = 0; i < seh_count; ++i) {
        RegisterSehClause sc;
        std::memcpy(&sc, data + offset, sizeof(sc));
        offset += sizeof(RegisterSehClause);

        rm.seh_clauses[i].flags = static_cast<interpreter::SEHFlags>(sc.flags);
        rm.seh_clauses[i].try_start_idx = sc.try_start_idx;
        rm.seh_clauses[i].try_end_idx = sc.try_end_idx;
        rm.seh_clauses[i].handler_start_idx = sc.handler_start_idx;
        rm.seh_clauses[i].handler_end_idx = sc.handler_end_idx;
        rm.seh_clauses[i].class_token = sc.class_token;
    }

    // Read CatchHandlerEntry array
    rm.catch_handler_entries.resize(catch_count);
    for (uint32_t i = 0; i < catch_count; ++i) {
        BinaryCatchHandlerEntry ce;
        std::memcpy(&ce, data + offset, sizeof(ce));
        offset += sizeof(BinaryCatchHandlerEntry);

        rm.catch_handler_entries[i].handler_start_idx = ce.handler_start_idx;
        rm.catch_handler_entries[i].exception_reg = ce.exception_reg;
        rm.catch_handler_entries[i].class_token = ce.class_token;
    }

    // Read IL offsets
    rm.il_offsets.resize(il_count);
    if (il_count > 0) {
        size_t bytes = il_count * sizeof(uint32_t);
        std::memcpy(rm.il_offsets.data(), data + offset, bytes);
        offset += bytes;
    }

    // Read stack map entries
    rm.stack_map.entries.resize(sm_count);
    for (uint32_t i = 0; i < sm_count; ++i) {
        BinaryStackMapEntry se;
        std::memcpy(&se, data + offset, sizeof(se));
        offset += sizeof(BinaryStackMapEntry);

        std::memcpy(rm.stack_map.entries[i].slot_regs, se.slot_regs, sizeof(se.slot_regs));
        std::memcpy(rm.stack_map.entries[i].local_regs, se.local_regs, sizeof(se.local_regs));
        rm.stack_map.entries[i].stack_depth = se.stack_depth;
    }

    return rm;
}

} // namespace chaos::il2cpp::jit
