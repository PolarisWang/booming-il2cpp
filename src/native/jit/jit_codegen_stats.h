#ifndef CHAOS_IL2CPP_JIT_CODEGEN_STATS_H_
#define CHAOS_IL2CPP_JIT_CODEGEN_STATS_H_

// ── Allocation-quality diagnostics for the JIT graph-coloring path ─────────
//
// Purpose: quantify how much of the register allocation actually keeps values
// in registers vs. spilling to the stack file.  The four accessor functions
// (LoadGpr / StoreGpr / LoadFpr / StoreFpr) in jit_codegen_memory.cpp choose
// between a colored x64 register and a stack slot; instrumentation here records
// which path was taken so we can find the hottest stack-round-trip opcodes.
//
// Gate: entirely disabled unless the env var CHAOS_IL2CPP_CODEGEN_STATS is set
// to a non-empty value.  When disabled, the per-access cost is a single always-
// false branch + a return (inlined), so it must not perturb production codegen.
//
// Output:  DumpCodegenStatsJson(path) writes a JSON report; see struct below.
//
// This file is intentionally header-only and self-contained (no dependency on
// the generator / codegen TUs) so it can be included from jit_codegen_memory.cpp
// and jit_codegen_generate.cpp without widening the include graph.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

namespace chaos::il2cpp::jit {

// Number of IROpCode values (ir_opcodes.h enumerates 0..110 inclusive).
static constexpr uint32_t kIROpCount = 111;

/// Per-method allocation outcomes recorded once at Generate().
struct CodegenMethodStats {
    uint32_t gpr_vreg_total = 0;   // candidate GPR virtual registers
    uint32_t gpr_vreg_colored = 0; // assigned a physical GPR (!= 0xFF)
    uint32_t gpr_vreg_spilled = 0; // fell through to stack (== 0xFF)
    uint32_t fpr_vreg_colored = 0;
    uint32_t fpr_vreg_spilled = 0;
    uint32_t n_instrs = 0;
};

/// Session-wide counters, aggregated across every compiled method.
///
/// "stack" means an operand crossed through the stack file (GprOff / FprOff) —
/// the round-trips graph coloring is meant to eliminate.  Registers-hit counters
/// are the good case (value already resident in a colored x64/XMM register).
struct CodegenStats {
    // GPR accessor path outcomes (LoadGpr / StoreGpr).
    uint64_t gpr_load_total = 0;
    uint64_t gpr_load_reg = 0;   // colored reg-to-reg move, no stack
    uint64_t gpr_load_stack = 0; // read through stack file
    uint64_t gpr_store_total = 0;
    uint64_t gpr_store_reg = 0;   // colored reg-to-reg move, no stack write
    uint64_t gpr_store_stack = 0; // write through to stack file
    // Caller-colored write-through: colored to a caller-saved reg but still
    // written to stack to survive a potential call clobber (only when
    // has_caller_clobber_).  T2.1 A1 eliminated these on call-free methods.
    uint64_t gpr_store_writethrough = 0;
    uint64_t fpr_load_total = 0;
    uint64_t fpr_load_reg = 0;
    uint64_t fpr_load_stack = 0;
    uint64_t fpr_store_total = 0;
    uint64_t fpr_store_reg = 0;
    uint64_t fpr_store_stack = 0;
    uint64_t fpr_store_writethrough = 0;

    // Per-method totals for the "how much spilled" headline metric.
    uint64_t methods_compiled = 0;
    uint64_t gpr_vreg_total = 0;
    uint64_t gpr_vreg_spilled = 0;
    uint64_t fpr_vreg_spilled = 0;

    // Stack-traffic summed per IROpCode, to rank the hottest round-trip opcodes.
    uint64_t opcode_stack_access[kIROpCount] = {};
    // Register traffic per IROpCode for the register-hit ratio.
    uint64_t opcode_reg_access[kIROpCount] = {};
};

inline CodegenStats& CodegenStatsInstance() noexcept {
    static CodegenStats s;
    return s;
}

/// True only when CHAOS_IL2CPP_CODEGEN_STATS is set.  Read once per Generate()
/// and cached on the NativeCodeGenerator; the hot accessors consult the cached
/// bool (a single predictable branch that is never taken in production).
inline bool CodegenStatsEnabled() noexcept {
    const char* e = std::getenv("CHAOS_IL2CPP_CODEGEN_STATS");
    return e != nullptr && e[0] != '\0';
}

/// Record a GPR accessor result.  `stacked` = true if this operand crossed the
/// stack file (round-trip); for stores, `write_through` = caller-colored but
/// still written to stack (a partial round-trip the A1 fix widens).
inline void RecordGprAccess(bool is_load, bool stacked, bool write_through, uint32_t opc) noexcept {
    if (opc >= kIROpCount)
        opc = kIROpCount - 1;
    CodegenStats& st = CodegenStatsInstance();
    if (is_load) {
        ++st.gpr_load_total;
        stacked ? ++st.gpr_load_stack : ++st.gpr_load_reg;
    } else {
        ++st.gpr_store_total;
        if (write_through)
            ++st.gpr_store_writethrough;
        else
            stacked ? ++st.gpr_store_stack : ++st.gpr_store_reg;
    }
    if (stacked)
        ++st.opcode_stack_access[opc];
    else
        ++st.opcode_reg_access[opc];
}

inline void RecordFprAccess(bool is_load, bool stacked, bool write_through, uint32_t opc) noexcept {
    if (opc >= kIROpCount)
        opc = kIROpCount - 1;
    CodegenStats& st = CodegenStatsInstance();
    if (is_load) {
        ++st.fpr_load_total;
        stacked ? ++st.fpr_load_stack : ++st.fpr_load_reg;
    } else {
        ++st.fpr_store_total;
        if (write_through)
            ++st.fpr_store_writethrough;
        else
            stacked ? ++st.fpr_store_stack : ++st.fpr_store_reg;
    }
    if (stacked)
        ++st.opcode_stack_access[opc];
    else
        ++st.opcode_reg_access[opc];
}

/// Record per-method coloring outcomes (called once at the end of Generate).
inline void RecordMethodStats(const CodegenMethodStats& m) noexcept {
    CodegenStats& st = CodegenStatsInstance();
    ++st.methods_compiled;
    st.gpr_vreg_total += m.gpr_vreg_total;
    st.gpr_vreg_spilled += m.gpr_vreg_spilled;
    st.fpr_vreg_spilled += m.fpr_vreg_spilled;
}

/// Write the aggregated session stats as JSON.  No-op when the gate is off.
/// Return 0 on success, -1 on gate-off or file open failure.
inline int DumpCodegenStatsJson(const char* path) noexcept {
    if (!CodegenStatsEnabled())
        return -1;
    const CodegenStats& st = CodegenStatsInstance();
    std::FILE* f = std::fopen(path, "w");
    if (!f)
        return -1;

    std::fprintf(f, "{\n");
    std::fprintf(f, "  \"gate\": \"CHAOS_IL2CPP_CODEGEN_STATS\",\n");
    std::fprintf(f, "  \"methods_compiled\": %llu,\n",
                 static_cast<unsigned long long>(st.methods_compiled));
    std::fprintf(f, "  \"gpr_vreg_total\": %llu,\n",
                 static_cast<unsigned long long>(st.gpr_vreg_total));
    std::fprintf(f, "  \"gpr_vreg_spilled\": %llu,\n",
                 static_cast<unsigned long long>(st.gpr_vreg_spilled));
    std::fprintf(f, "  \"fpr_vreg_spilled\": %llu,\n",
                 static_cast<unsigned long long>(st.fpr_vreg_spilled));
    std::fprintf(f, "  \"gpr_load\": { \"total\": %llu, \"reg\": %llu, \"stack\": %llu },\n",
                 static_cast<unsigned long long>(st.gpr_load_total),
                 static_cast<unsigned long long>(st.gpr_load_reg),
                 static_cast<unsigned long long>(st.gpr_load_stack));
    std::fprintf(f, "  \"gpr_store\": { \"total\": %llu, \"reg\": %llu, \"stack\": %llu, \"writethrough\": %llu },\n",
                 static_cast<unsigned long long>(st.gpr_store_total),
                 static_cast<unsigned long long>(st.gpr_store_reg),
                 static_cast<unsigned long long>(st.gpr_store_stack),
                 static_cast<unsigned long long>(st.gpr_store_writethrough));
    std::fprintf(f, "  \"fpr_load\": { \"total\": %llu, \"reg\": %llu, \"stack\": %llu },\n",
                 static_cast<unsigned long long>(st.fpr_load_total),
                 static_cast<unsigned long long>(st.fpr_load_reg),
                 static_cast<unsigned long long>(st.fpr_load_stack));
    std::fprintf(f, "  \"fpr_store\": { \"total\": %llu, \"reg\": %llu, \"stack\": %llu, \"writethrough\": %llu },\n",
                 static_cast<unsigned long long>(st.fpr_store_total),
                 static_cast<unsigned long long>(st.fpr_store_reg),
                 static_cast<unsigned long long>(st.fpr_store_stack),
                 static_cast<unsigned long long>(st.fpr_store_writethrough));

    // Top stack-round-trip opcodes by descending stack access count.
    std::fprintf(f, "  \"opcode_stack_rank\": [\n");
    std::uint32_t order[kIROpCount];
    for (std::uint32_t i = 0; i < kIROpCount; ++i)
        order[i] = i;
    std::sort(order, order + kIROpCount,
              [&](std::uint32_t a, std::uint32_t b) {
                  return st.opcode_stack_access[a] > st.opcode_stack_access[b];
              });
    bool first = true;
    for (std::uint32_t i = 0; i < kIROpCount; ++i) {
        const std::uint32_t opc = order[i];
        const std::uint64_t stack_n = st.opcode_stack_access[opc];
        const std::uint64_t reg_n = st.opcode_reg_access[opc];
        if (stack_n == 0 && reg_n == 0)
            continue;
        if (!first)
            std::fprintf(f, ",\n");
        first = false;
        std::fprintf(f, "    { \"opcode\": %u, \"stack\": %llu, \"reg\": %llu, \"stack_pct\": %.1f }",
                     opc, static_cast<unsigned long long>(stack_n),
                     static_cast<unsigned long long>(reg_n),
                     (stack_n + reg_n) ? (100.0 * double(stack_n) / double(stack_n + reg_n)) : 0.0);
    }
    std::fprintf(f, "\n  ]\n}\n");
    std::fclose(f);
    return 0;
}

} // namespace chaos::il2cpp::jit

#endif // CHAOS_IL2CPP_JIT_CODEGEN_STATS_H_
