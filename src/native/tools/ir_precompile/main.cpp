// ir_precompile — Pre-compile AotCoreIr JSON into register-based IR binary (.regir)
//
// Reads aot-core-ir.json, deserializes each method, runs the register allocator,
// and writes a compact .regir binary file that the PatchLoader can use directly
// to bypass JSON deserialization + register allocation at runtime.
//
// Usage:
//   ir_precompile --input <aot-core-ir.json> --output <output.regir>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csetjmp>
#include <string>
#include <vector>

#include <chaos/json_reader.h>
#include <interpreter_vm.h>
#include <ir_reg_alloc.h>
#include <aot_core_ir_reader.h>
#include "codegen_bridge.h"

// ── Stub resolve callbacks ──────────────────────────────────────────────────
// These run at precompile time where we don't have a live runtime.
// Returning nullptr means call_target resolution is deferred to runtime,
// which is fine since register allocation only cares about instruction shapes.
static void* StubResolveSubjectId(const char*, void*) noexcept {
    return nullptr;
}

static void* StubResolveDirectFn(const char*, void*) noexcept {
    return nullptr;
}

// ── Stub external symbols —──────────────────────────────────────────────────
// The ir_precompile tool links against chaos_interpreter and chaos_runtime_core,
// which reference symbols normally provided by AOT-generated code (entry.exe).
// We provide zero-initialized stubs since the precompile tool doesn't use them.

extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;

extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// g_chaos_exception_jmp is thread_local jmp_buf* defined in exception_helpers.cpp.
namespace chaos::il2cpp::runtime_core {
thread_local jmp_buf* g_chaos_exception_jmp = nullptr;
}

// ── .regir binary format ────────────────────────────────────────────────────
// uint32_t method_count
// [for each method]:
//     uint32_t subject_id_hash      // 4-byte lookup key (FNV-1a of subject ID)
//     uint16_t max_regs
//     uint16_t instr_count
//     uint16_t seh_count
//     uint16_t padding
//     RegisterInstruction instrs[instr_count]   // 16 bytes each
//     SEHClauseCompact seh_clauses[seh_count]   // 24 bytes each

static uint32_t Fnv1aHash(const char* str, size_t len) noexcept {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(str[i]);
        hash *= 16777619u;
    }
    return hash;
}

#pragma pack(push, 1)
struct RegirMethodHeader {
    uint32_t subject_id_hash;
    uint16_t max_regs;
    uint16_t instr_count;
    uint16_t seh_count;
    uint16_t padding;  // align to 4 bytes
};
#pragma pack(pop)
static_assert(sizeof(RegirMethodHeader) == 12,
    "RegirMethodHeader must be 12 bytes");

// ── SEHClauseCompact (from patch_data.h, redefined here for self-containment) ──
// 24 bytes: 6 × uint32_t
#pragma pack(push, 1)
struct SEHClauseCompact {
    uint32_t try_start;
    uint32_t try_end;
    uint32_t handler_start;
    uint32_t catch_type_token_or_parent_vreg;
    uint32_t flags;
    uint32_t reserve;
};
#pragma pack(pop)
static_assert(sizeof(SEHClauseCompact) == 24,
    "SEHClauseCompact must be 24 bytes");

static SEHClauseCompact ConvertSEHClause(
    const chaos::il2cpp::interpreter::SEHClause& clause) noexcept {
    SEHClauseCompact compact = {};
    compact.try_start  = static_cast<uint32_t>(clause.try_start_idx);
    compact.try_end    = static_cast<uint32_t>(clause.try_end_idx);
    compact.handler_start = static_cast<uint32_t>(clause.handler_start_idx);
    // Not the same in all cases, but for serialization we pack the class_token
    // into catch_type_token_or_parent_vreg. The runtime reader will reinterpret.
    compact.catch_type_token_or_parent_vreg = clause.class_token;
    compact.flags = static_cast<uint32_t>(clause.flags);
    compact.reserve = 0;
    return compact;
}

// ── File I/O helpers ────────────────────────────────────────────────────────

static std::string ReadFile(const char* path) noexcept {
    FILE* f = std::fopen(path, "rb");
    if (!f) {
        std::fprintf(stderr, "Error: cannot open input file: %s\n", path);
        return {};
    }
    std::fseek(f, 0, SEEK_END);
    long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (size <= 0) {
        std::fclose(f);
        std::fprintf(stderr, "Error: empty input file: %s\n", path);
        return {};
    }
    std::string content(static_cast<size_t>(size), '\0');
    size_t read = std::fread(content.data(), 1, static_cast<size_t>(size), f);
    std::fclose(f);
    if (static_cast<long>(read) != size) {
        std::fprintf(stderr, "Error: short read from: %s\n", path);
        return {};
    }
    return content;
}

static bool WriteFile(const char* path, const std::vector<uint8_t>& data) noexcept {
    FILE* f = std::fopen(path, "wb");
    if (!f) {
        std::fprintf(stderr, "Error: cannot open output file: %s\n", path);
        return false;
    }
    size_t written = std::fwrite(data.data(), 1, data.size(), f);
    std::fclose(f);
    if (written != data.size()) {
        std::fprintf(stderr, "Error: short write to: %s\n", path);
        return false;
    }
    return true;
}

// ── Main ────────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    const char* input_path = nullptr;
    const char* output_path = nullptr;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            std::fprintf(stderr, "Usage: ir_precompile --input <aot-core-ir.json> --output <output.regir>\n");
            return 0;
        }
    }

    if (!input_path || !output_path) {
        std::fprintf(stderr, "Error: --input and --output are required\n");
        std::fprintf(stderr, "Usage: ir_precompile --input <aot-core-ir.json> --output <output.regir>\n");
        return 1;
    }

    std::string json_text = ReadFile(input_path);
    if (json_text.empty()) {
        return 1;
    }

    // Parse the aot-core-ir.json (top-level object with "methods" array).
    auto parse = chaos::il2cpp::json::JsonParser::Parse(
        json_text.data(), json_text.size());
    if (parse.error || !parse.value.IsObject()) {
        std::fprintf(stderr, "Error: failed to parse JSON (not a valid object)\n");
        return 1;
    }

    const auto& root = parse.value;
    auto methods_arr = chaos::il2cpp::json::JsonParser::FindKey(root, "methods");
    if (!methods_arr.IsArray()) {
        std::fprintf(stderr, "Error: no 'methods' array found in JSON\n");
        return 1;
    }

    // ── First pass: collect all method JSON strings ─────────────────────────
    struct MethodEntry {
        std::string subject_id;   // for hash
        std::string json;         // raw JSON text
    };
    std::vector<MethodEntry> methods;
    size_t total_ir_size = 0;

    chaos::il2cpp::json::JsonParser::ForEachArrayElement(methods_arr,
        [&](size_t index, const chaos::il2cpp::json::JsonValue& elem) -> bool {
            // Get the raw JSON text for this method.
            if (!elem.IsObject()) return true;

            // Extract subjectId for the lookup hash.
            auto sid_val = chaos::il2cpp::json::JsonParser::FindKey(elem, "subjectId");
            std::string sid;
            if (sid_val.IsString() && sid_val.string_value) {
                sid.assign(sid_val.string_value, sid_val.string_length);
            }

            // Serialize this element back to JSON.
            std::string method_json(elem.data_start, elem.data_length);
            methods.push_back({std::move(sid), std::move(method_json)});

            return true;  // continue iteration
        });

    std::fprintf(stderr, "[ir_precompile] Found %zu methods in input\n", methods.size());

    // ── Second pass: deserialize + allocate registers + serialize ──────────
    std::vector<uint8_t> output;
    uint32_t method_count = static_cast<uint32_t>(methods.size());
    output.insert(output.end(),
        reinterpret_cast<const uint8_t*>(&method_count),
        reinterpret_cast<const uint8_t*>(&method_count) + sizeof(method_count));

    size_t success_count = 0;
    size_t fail_count = 0;

    for (const auto& entry : methods) {
        // Deserialize AotCoreIr JSON → IRMethod (stack IR).
        auto ir = chaos::il2cpp::runtime_core::DeserializeAotCoreIrMethod(
            entry.json.data(),
            entry.json.size(),
            StubResolveSubjectId, nullptr,
            StubResolveDirectFn, nullptr);

        // Run register allocator: IRMethod → RegisterMethod.
        auto reg_method = chaos::il2cpp::interpreter::AllocateRegisters(ir);

        uint32_t instr_count = static_cast<uint32_t>(reg_method.instructions.size());
        uint32_t seh_count   = static_cast<uint32_t>(reg_method.seh_clauses.size());
        uint32_t max_regs    = reg_method.max_regs;
        uint32_t hash        = Fnv1aHash(entry.subject_id.data(), entry.subject_id.size());

        // Write method header.
        RegirMethodHeader hdr = {};
        hdr.subject_id_hash = hash;
        hdr.max_regs  = static_cast<uint16_t>(max_regs);
        hdr.instr_count = static_cast<uint16_t>(instr_count);
        hdr.seh_count = static_cast<uint16_t>(seh_count);
        hdr.padding  = 0;
        output.insert(output.end(),
            reinterpret_cast<const uint8_t*>(&hdr),
            reinterpret_cast<const uint8_t*>(&hdr) + sizeof(hdr));

        // Write RegisterInstruction array (16 bytes each).
        output.insert(output.end(),
            reinterpret_cast<const uint8_t*>(reg_method.instructions.data()),
            reinterpret_cast<const uint8_t*>(reg_method.instructions.data()) +
                instr_count * sizeof(chaos::il2cpp::interpreter::RegisterInstruction));

        // Write SEHClauseCompact array (24 bytes each).
        for (const auto& clause : reg_method.seh_clauses) {
            auto compact = ConvertSEHClause(clause);
            output.insert(output.end(),
                reinterpret_cast<const uint8_t*>(&compact),
                reinterpret_cast<const uint8_t*>(&compact) + sizeof(compact));
        }

        total_ir_size += sizeof(hdr) +
            static_cast<size_t>(instr_count) * sizeof(chaos::il2cpp::interpreter::RegisterInstruction) +
            static_cast<size_t>(seh_count) * sizeof(SEHClauseCompact);
        ++success_count;
    }

    // ── Write output ────────────────────────────────────────────────────────
    if (!WriteFile(output_path, output)) {
        return 1;
    }

    double avg_instr = success_count > 0
        ? static_cast<double>(total_ir_size) / (success_count * 16.0)
        : 0.0;

    std::fprintf(stderr,
        "[ir_precompile] Done: %zu/%zu methods converted, "
        "%zu failed, %zu bytes output (avg %.1f instr/method)\n",
        success_count, methods.size(), fail_count, output.size(), avg_instr);
    return 0;
}