// aot_core_ir_reader.cpp — Deserialize AotCoreIr JSON into IRMethod.
#include "aot_core_ir_reader.h"
#include <chaos/json_reader.h>
#include <chaos/native_types.h>

#include <reflection_query_model.h>

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

namespace chaos::il2cpp::runtime_core {

// ── Helpers for json_reader convenience ──────────────────────────────────

static int64_t JsonIntOr(const json::JsonValue& val, int64_t default_val) {
    if (val.kind == json::JsonValueKind::Int64) return val.int64_value;
    return default_val;
}

static double JsonDoubleOr(const json::JsonValue& val, double default_val) {
    if (val.kind == json::JsonValueKind::Double) return val.double_value;
    if (val.kind == json::JsonValueKind::Int64) return static_cast<double>(val.int64_value);
    return default_val;
}

static const char* JsonStringOr(const json::JsonValue& val) {
    if (val.kind == json::JsonValueKind::String) return val.string_value;
    return nullptr;
}

// Count parameters from a callee signature string.
// Format: "Assembly/Type::Method:ReturnType(Param1,Param2,...)"
// Returns the parameter count, or -1 if the string can't be parsed.
static int CountParametersFromCallee(const json::JsonValue& callee_val) {
    if (callee_val.kind != json::JsonValueKind::String ||
        callee_val.string_value == nullptr || callee_val.string_length == 0)
        return -1;
    std::string s(callee_val.string_value, callee_val.string_length);

    // Find the last '(' which starts the parameter list.
    auto paren = s.rfind('(');
    if (paren == std::string::npos) return -1;

    // Empty parens means 0 parameters.
    if (paren + 1 >= s.size() || s[paren + 1] == ')') return 0;

    // Count top-level commas (not nested in <...> for generics).
    int count = 1;
    int depth = 0;
    for (size_t i = paren + 1; i < s.size() && s[i] != ')'; ++i) {
        if (s[i] == '<') ++depth;
        else if (s[i] == '>') --depth;
        else if (s[i] == ',' && depth == 0) ++count;
    }
    return count;
}

// Extract the return type name from a subjectId string.
// SubjectId format: "Assembly/Type::MethodName:ReturnType(Params...)"
// Returns empty string if no return type annotation is present.
static std::string ExtractReturnTypeFromSubjectId(const std::string& subject_id) {
    auto found = subject_id.rfind("::");
    if (found == std::string::npos) return "";

    auto method_part = subject_id.substr(found + 2);
    auto colon = method_part.rfind(':');
    auto paren = method_part.find('(');

    if (colon == std::string::npos || colon == 0) return "";
    // The colon must be before the paren (method-return separator),
    // not part of the params area.
    if (paren != std::string::npos && colon > paren) return "";

    size_t ret_start = colon + 1;
    size_t ret_end = (paren != std::string::npos) ? paren : method_part.size();
    if (ret_start >= ret_end) return "";

    return method_part.substr(ret_start, ret_end - ret_start);
}

// Map a .NET return type name to an interpreter ValueTag.
// Used during IR deserialization to set direct_ret_tag for direct_fn calls.
static uint8_t InferValueTagFromReturnTypeName(const char* type_name) {
    if (type_name == nullptr) return 0xFF;

    // ECMA primitive types → Int32
    if (std::strcmp(type_name, "System.Char") == 0 ||
        std::strcmp(type_name, "System.Byte") == 0 ||
        std::strcmp(type_name, "System.SByte") == 0 ||
        std::strcmp(type_name, "System.Int16") == 0 ||
        std::strcmp(type_name, "System.UInt16") == 0 ||
        std::strcmp(type_name, "System.Int32") == 0 ||
        std::strcmp(type_name, "System.UInt32") == 0 ||
        std::strcmp(type_name, "System.Boolean") == 0)
        return static_cast<uint8_t>(interpreter::ValueTag::Int32);

    if (std::strcmp(type_name, "System.Int64") == 0 ||
        std::strcmp(type_name, "System.UInt64") == 0)
        return static_cast<uint8_t>(interpreter::ValueTag::Int64);

    if (std::strcmp(type_name, "System.Single") == 0)
        return static_cast<uint8_t>(interpreter::ValueTag::Float32);

    if (std::strcmp(type_name, "System.Double") == 0)
        return static_cast<uint8_t>(interpreter::ValueTag::Float64);

    if (std::strcmp(type_name, "System.Void") == 0)
        return static_cast<uint8_t>(interpreter::ValueTag::Void);

    // Default: ObjectRef for String, Object, DateTime, Decimal, struct, etc.
    return static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
}

static int ParseOpCode(const json::JsonValue& instr_obj) {
    auto op_code_val = json::JsonParser::FindKey(instr_obj, "opCode");
    if (op_code_val.kind == json::JsonValueKind::Int64)
        return static_cast<int>(op_code_val.int64_value);
    // Fallback: try "op" string field and map (backward compat)
    // Not implemented — new format requires numeric opCode.
    return -1;
}

// ── IlOffset → instruction index mapping ─────────────────────────────────
// Used to convert branch target byte offsets to instruction indices.

struct OffsetMapEntry {
    int      il_offset;
    size_t   instruction_index;
};

static size_t ResolveBranchTarget(
    int target_offset,
    const OffsetMapEntry* offset_map,
    size_t map_size)
{
    if (offset_map == nullptr || map_size == 0) return 0;
    for (size_t i = 0; i < map_size; ++i) {
        if (offset_map[i].il_offset == target_offset)
            return offset_map[i].instruction_index;
    }
    // Fallback: find the nearest offset <= target_offset.
    // This handles branch targets that point into the middle of an
    // instruction (shouldn't happen with valid IL, but be safe).
    size_t best = 0;
    for (size_t i = 0; i < map_size; ++i) {
        if (offset_map[i].il_offset <= target_offset)
            best = i;
    }
    return offset_map[best].instruction_index;
}

// ── Main deserialization ─────────────────────────────────────────────────

interpreter::IRMethod DeserializeAotCoreIrMethod(
    const char* json,
    size_t length,
    ResolveSubjectIdFn resolve_fn,
    void* resolve_ctx,
    ResolveSubjectIdFn resolve_direct_fn,
    void* direct_ctx)
{
    interpreter::IRMethod result;
    if (json == nullptr || length == 0) {
        result.instructions.push_back({});  // single Ret
        return result;
    }

    // Parse top-level object.
    auto parse = json::JsonParser::Parse(json, length);
    if (parse.error || !parse.value.IsObject()) {
        result.instructions.push_back({});
        return result;
    }

    const auto& root = parse.value;

    // ── Step 1: Parse instructions ──────────────────────────────────────
    auto instructions_arr = json::JsonParser::FindKey(root, "instructions");
    if (!instructions_arr.IsArray()) {
        result.instructions.push_back({});
        return result;
    }

    // First pass: collect offset→index map and build IRInstructions.
    std::vector<OffsetMapEntry> offset_map;
    std::vector<interpreter::IRInstruction> raw_instructions;

    json::JsonParser::ForEachArrayElement(instructions_arr,
        [&](size_t index, const json::JsonValue& elem) -> bool {
            if (!elem.IsObject()) return true;  // skip invalid

            int opcode = ParseOpCode(elem);
            if (opcode < 0) {
                // Skip invalid opcode — insert Ret as sentinel.
                raw_instructions.push_back({});
                return true;
            }

            interpreter::IRInstruction instr;
            instr.op_code = static_cast<interpreter::IROpCode>(opcode);

            // Parse ilOffset for branch target resolution.
            auto il_offset_val = json::JsonParser::FindKey(elem, "ilOffset");
            int il_offset = static_cast<int>(JsonIntOr(il_offset_val, 0));
            offset_map.push_back({il_offset, index});

            // Parse operand (object? — can be int, string, or null).
            auto operand_val = json::JsonParser::FindKey(elem, "operand");

            // ── Map operands based on opcode ───────────────────────────
            switch (instr.op_code) {
            case interpreter::IROpCode::LdcI4:
                // Operand is the int32 constant.
                instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::LdcI8:
                instr.immediate_i8 = JsonIntOr(operand_val, 0);
                break;

            case interpreter::IROpCode::LdcR4:
                // Float32 packed as int32 in operand.
                {
                    int32_t bits = static_cast<int32_t>(JsonIntOr(operand_val, 0));
                    std::memcpy(&instr.immediate_r8, &bits, sizeof(bits));
                }
                break;

            case interpreter::IROpCode::LdcR8:
                instr.immediate_r8 = JsonDoubleOr(operand_val, 0.0);
                break;

            case interpreter::IROpCode::LdStr:
                instr.string_operand = JsonStringOr(operand_val);
                break;

            case interpreter::IROpCode::LdArg:
            case interpreter::IROpCode::LdLoc:
            case interpreter::IROpCode::StLoc:
            case interpreter::IROpCode::StArg:
            case interpreter::IROpCode::LdArgA:
            case interpreter::IROpCode::LdLocA:
                // operand_index from the Op string (e.g. "ldarg.0").
                // For now, use the integer operand value if available.
                instr.operand_index = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::LdFld:
            case interpreter::IROpCode::StFld:
                // field_offset comes from TargetReference reference resolution.
                // immediate_i4 carries the token/type info.
                instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                instr.field_offset = static_cast<CHAOS_IL2CPP_SIZE>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::LdSFld:
            case interpreter::IROpCode::StSFld:
                instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                instr.field_offset = static_cast<CHAOS_IL2CPP_SIZE>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::Call:
            case interpreter::IROpCode::CallVirt:
            case interpreter::IROpCode::CallBridge:
            case interpreter::IROpCode::NewObj:
                // arg_count from JSON operand or default to 0.
                // Will be overridden by targetParameterCount below if present,
                // or derived from callee signature as fallback.
                instr.arg_count = static_cast<CHAOS_IL2CPP_UINT32>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::CallVirtConstrained:
                instr.arg_count = static_cast<CHAOS_IL2CPP_UINT32>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::Box:
            case interpreter::IROpCode::Unbox:
            case interpreter::IROpCode::CastClass:
            case interpreter::IROpCode::IsInst:
            case interpreter::IROpCode::NewArr:
            case interpreter::IROpCode::InitObj:
            case interpreter::IROpCode::LdToken:
            case interpreter::IROpCode::LdFtn:
            case interpreter::IROpCode::LdVirtFtn:
                // These use call_target from TargetReference resolution.
                // immediate_i4 may carry token data.
                instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::LdElem:
            case interpreter::IROpCode::StElem:
            case interpreter::IROpCode::LdInd:
            case interpreter::IROpCode::StInd:
            case interpreter::IROpCode::LdObj:
            case interpreter::IROpCode::StObj:
            case interpreter::IROpCode::LdElemA:
            case interpreter::IROpCode::SizeOf:
            case interpreter::IROpCode::LocAlloc:
                // Type info via immediate_i4.
                instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                break;

            case interpreter::IROpCode::Switch:
                // Operand is the number of cases.
                instr.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(
                    JsonIntOr(operand_val, 0));
                break;

            // ── Branch opcodes: operand is the IL byte offset target ──
            // immediate_i4 is used later by Step 2 (ResolveBranchTarget)
            // to convert byte offset → instruction index.
            case interpreter::IROpCode::Br:
            case interpreter::IROpCode::BrTrue:
            case interpreter::IROpCode::BrFalse:
            case interpreter::IROpCode::Beq:
            case interpreter::IROpCode::Blt:
            case interpreter::IROpCode::Bgt:
            case interpreter::IROpCode::Ble:
            case interpreter::IROpCode::Bge:
            case interpreter::IROpCode::BneUn:
            case interpreter::IROpCode::BgeUn:
            case interpreter::IROpCode::BgtUn:
            case interpreter::IROpCode::BleUn:
            case interpreter::IROpCode::BltUn:
            case interpreter::IROpCode::Leave:
                instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                    JsonIntOr(operand_val, 0));
                break;

            default:
                // Most opcodes (add, sub, ret, etc.) have no operand.
                break;
            }

            // ── Resolve call_target ──
            // For call-like opcodes, priority is: callee (most specific,
            // already rewritten by C# lowering for constrained overrides)
            // → targetReference.subjectId → targetSymbol.
            // For non-call opcodes (NewObj, Box, etc.), targetReference
            // remains the primary resolution source.
            if (instr.op_code == interpreter::IROpCode::Call ||
                instr.op_code == interpreter::IROpCode::CallVirt ||
                instr.op_code == interpreter::IROpCode::CallBridge ||
                instr.op_code == interpreter::IROpCode::NewObj ||
                instr.op_code == interpreter::IROpCode::CallVirtConstrained ||
                instr.op_code == interpreter::IROpCode::Box ||
                instr.op_code == interpreter::IROpCode::Unbox ||
                instr.op_code == interpreter::IROpCode::CastClass ||
                instr.op_code == interpreter::IROpCode::IsInst ||
                instr.op_code == interpreter::IROpCode::NewArr ||
                instr.op_code == interpreter::IROpCode::InitObj ||
                instr.op_code == interpreter::IROpCode::LdToken ||
                instr.op_code == interpreter::IROpCode::LdFtn ||
                instr.op_code == interpreter::IROpCode::LdVirtFtn ||
                instr.op_code == interpreter::IROpCode::LdSFld ||
                instr.op_code == interpreter::IROpCode::StSFld)
            {
                bool const is_call_like =
                    instr.op_code == interpreter::IROpCode::Call ||
                    instr.op_code == interpreter::IROpCode::CallVirt ||
                    instr.op_code == interpreter::IROpCode::CallBridge ||
                    instr.op_code == interpreter::IROpCode::CallVirtConstrained;

                if (is_call_like)
                {
                    // 1st priority: callee — C# lowering rewrites this to the
                    // value type's own override for constrained callvirt
                    // (e.g., Guid::GetHashCode instead of Object::GetHashCode).
                    auto callee_val = json::JsonParser::FindKey(elem, "callee");
                    if (callee_val.IsString() && resolve_fn != nullptr)
                    {
                        std::string callee_subject(callee_val.string_value, callee_val.string_length);
                        if (!callee_subject.empty())
                        {
                            instr.call_target = resolve_fn(callee_subject.c_str(), resolve_ctx);
                        }
                    }

                    // 2nd priority: targetReference.subjectId
                    // Note: JSON reader's string_value may NOT be null-terminated
                    // for non-escaped strings (points into raw JSON buffer).
                    if (instr.call_target == nullptr)
                    {
                        auto target_ref = json::JsonParser::FindKey(elem, "targetReference");
                        if (target_ref.IsObject() && resolve_fn != nullptr)
                        {
                            auto subject_id = json::JsonParser::FindKey(target_ref, "subjectId");
                            if (subject_id.IsString())
                            {
                                std::string sid(subject_id.string_value, subject_id.string_length);
                                if (!sid.empty())
                                    instr.call_target = resolve_fn(sid.c_str(), resolve_ctx);
                            }
                        }
                    }

                    // 3rd priority: targetSymbol (direct AOT symbol).
                    if (instr.call_target == nullptr)
                    {
                        auto target_sym = json::JsonParser::FindKey(elem, "targetSymbol");
                        if (target_sym.IsString() && resolve_fn != nullptr)
                        {
                            std::string sym(target_sym.string_value, target_sym.string_length);
                            if (!sym.empty())
                                instr.call_target = resolve_fn(sym.c_str(), resolve_ctx);
                        }
                    }
                }
                else
                {
                    // Non-call opcodes: resolve from targetReference → targetSymbol.
                    auto target_ref = json::JsonParser::FindKey(elem, "targetReference");
                    if (target_ref.IsObject() && resolve_fn != nullptr)
                    {
                        auto subject_id = json::JsonParser::FindKey(target_ref, "subjectId");
                        if (subject_id.IsString())
                        {
                            std::string sid(subject_id.string_value, subject_id.string_length);
                            if (!sid.empty())
                                instr.call_target = resolve_fn(sid.c_str(), resolve_ctx);
                        }
                    }

                    // Also try targetSymbol (direct AOT symbol).
                    if (instr.call_target == nullptr)
                    {
                        auto target_sym = json::JsonParser::FindKey(elem, "targetSymbol");
                        if (target_sym.IsString() && resolve_fn != nullptr)
                        {
                            std::string sym(target_sym.string_value, target_sym.string_length);
                            if (!sym.empty())
                                instr.call_target = resolve_fn(sym.c_str(), resolve_ctx);
                        }
                    }
                }

                // For type-token opcodes (Box, CastClass, IsInst), populate
                // immediate_i4 from the resolved type descriptor's metadata_token.
                // The JSON "operand" carries a SubjectId string (not an integer), so
                // JsonIntOr at the lines above returned 0.  Decoding the resolved
                // TypeInfoHandle gives us the actual metadata_token for MIC lookups.
                // Only safe for opcodes where call_target is known to be TypeInfoHandle
                // (not MethodInfoHandle or FieldInfoHandle).
                if ((instr.op_code == interpreter::IROpCode::Box ||
                     instr.op_code == interpreter::IROpCode::CastClass ||
                     instr.op_code == interpreter::IROpCode::IsInst ||
                     instr.op_code == interpreter::IROpCode::NewArr) &&
                    instr.call_target != nullptr) {
                    auto* type_desc = TryDecodeReflectionQueryTypeHandle(
                        reinterpret_cast<TypeInfoHandle>(instr.call_target));
                    if (type_desc != nullptr) {
                        instr.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(
                            type_desc->metadata_token);
                    }
                }

                // For instance calls, check callee metadata.
                auto callee = json::JsonParser::FindKey(elem, "callee");
                if (callee.IsString()) {
                    instr.is_instance_call = true;
                }

                // For NewObj, also resolve arg_count from operand if not set.
                auto op_val = json::JsonParser::FindKey(elem, "operand");
                if (op_val.kind == json::JsonValueKind::Int64 && instr.op_code == interpreter::IROpCode::NewObj) {
                    // operand for newobj is typically the constructor arg count
                }

                // Try targetParameterCount for call instructions.
                if (instr.op_code == interpreter::IROpCode::Call ||
                    instr.op_code == interpreter::IROpCode::CallVirt ||
                    instr.op_code == interpreter::IROpCode::CallBridge)
                {
                    auto param_count = json::JsonParser::FindKey(elem, "targetParameterCount");
                    if (param_count.kind == json::JsonValueKind::Int64) {
                        instr.arg_count = static_cast<CHAOS_IL2CPP_UINT32>(param_count.int64_value);
                    } else {
                        // Fallback: derive arg_count from callee signature.
                        int derived = CountParametersFromCallee(callee);
                        if (derived >= 0) {
                            instr.arg_count = static_cast<CHAOS_IL2CPP_UINT32>(derived);
                        }
                    }
                }

                // ── AotDirectDispatch: resolve direct_fn from subjectId ──
                // When the resolve_direct_fn callback is provided, look up the
                // call's subjectId in the kAotDirectFnTable to get the pre-resolved
                // chaos_external_runtime_* function pointer.
                if (instr.op_code == interpreter::IROpCode::Call &&
                    resolve_direct_fn != nullptr)
                {
                    auto callee = json::JsonParser::FindKey(elem, "callee");
                    if (callee.kind == json::JsonValueKind::String &&
                        callee.string_value != nullptr && callee.string_length > 0) {
                        // JSON reader's string_value may NOT be null-terminated
                        // for non-escaped strings (points into raw JSON buffer).
                        // Create a proper C string for strcmp-based resolution.
                        std::string callee_str(callee.string_value, callee.string_length);
                        instr.direct_fn = resolve_direct_fn(callee_str.c_str(), direct_ctx);

                        // Infer return type tag from subjectId for direct_fn calls.
                        // This lets Handle_Call push the correct ValueTag without
                        // runtime string parsing or reflection queries.
                        if (instr.direct_fn != nullptr) {
                            auto ret_type = ExtractReturnTypeFromSubjectId(callee_str);
                            if (!ret_type.empty()) {
                                instr.direct_ret_tag = InferValueTagFromReturnTypeName(ret_type.c_str());
                            }
                        }
                    }
                }
            }

            raw_instructions.push_back(instr);
            return true;  // continue iteration
        });

    // ── Step 2: Resolve branch targets ────────────────────────────────
    // Branch opcodes have Operand = byte offset. Convert to instruction index.
    if (!offset_map.empty()) {
        for (auto& instr : raw_instructions) {
            switch (instr.op_code) {
            case interpreter::IROpCode::Br:
            case interpreter::IROpCode::BrTrue:
            case interpreter::IROpCode::BrFalse:
            case interpreter::IROpCode::Beq:
            case interpreter::IROpCode::Blt:
            case interpreter::IROpCode::Bgt:
            case interpreter::IROpCode::Ble:
            case interpreter::IROpCode::Bge:
            case interpreter::IROpCode::BneUn:
            case interpreter::IROpCode::BgeUn:
            case interpreter::IROpCode::BgtUn:
            case interpreter::IROpCode::BleUn:
            case interpreter::IROpCode::BltUn:
            case interpreter::IROpCode::Leave:
                instr.branch_target = ResolveBranchTarget(
                    instr.immediate_i4,
                    offset_map.data(), offset_map.size());
                break;
            case interpreter::IROpCode::Switch:
                // Switch targets will be resolved in a second pass if needed.
                // For now, branch_target stores the default target.
                break;
            default:
                break;
            }
        }
    }

    // ── Step 3: Parse exception regions ────────────────────────────────
    auto exc_regions = json::JsonParser::FindKey(root, "exceptionRegions");
    if (exc_regions.IsArray()) {
        json::JsonParser::ForEachArrayElement(exc_regions,
            [&](size_t /*index*/, const json::JsonValue& elem) -> bool {
                if (!elem.IsObject()) return true;

                interpreter::SEHClause clause = {};

                // Parse handling kind.
                auto kind = json::JsonParser::FindKey(elem, "handlingKindCode");
                if (kind.IsString()) {
                    std::string kind_str(kind.string_value, kind.string_length);
                    if (kind_str == "Catch" || kind_str == "Exception")
                        clause.flags = interpreter::SEHFlags::Exception;
                    else if (kind_str == "Finally")
                        clause.flags = interpreter::SEHFlags::Finally;
                    else if (kind_str == "Fault")
                        clause.flags = interpreter::SEHFlags::Fault;
                    else if (kind_str == "Filter")
                        clause.flags = interpreter::SEHFlags::Filter;
                }

                // Parse try/handler offsets and convert to instruction indices.
                auto try_off = json::JsonParser::FindKey(elem, "tryOffset");
                auto try_len = json::JsonParser::FindKey(elem, "tryLength");
                auto handler_off = json::JsonParser::FindKey(elem, "handlerOffset");
                auto handler_len = json::JsonParser::FindKey(elem, "handlerLength");

                int try_offset = static_cast<int>(JsonIntOr(try_off, 0));
                int try_length = static_cast<int>(JsonIntOr(try_len, 0));
                int handler_offset = static_cast<int>(JsonIntOr(handler_off, 0));
                int handler_length = static_cast<int>(JsonIntOr(handler_len, 0));

                // Convert byte offsets to instruction indices.
                clause.try_start_idx = ResolveBranchTarget(
                    try_offset, offset_map.data(), offset_map.size());
                clause.try_end_idx = ResolveBranchTarget(
                    try_offset + try_length, offset_map.data(), offset_map.size());
                clause.handler_start_idx = ResolveBranchTarget(
                    handler_offset, offset_map.data(), offset_map.size());
                clause.handler_end_idx = ResolveBranchTarget(
                    handler_offset + handler_length, offset_map.data(), offset_map.size());

                // Parse catch type.
                auto catch_type = json::JsonParser::FindKey(elem, "catchTypeSubjectId");
                if (catch_type.IsString() && resolve_fn != nullptr) {
                    std::string type_sid(catch_type.string_value, catch_type.string_length);
                    if (!type_sid.empty()) {
                        void* raw_handle = resolve_fn(type_sid.c_str(), resolve_ctx);
                        if (raw_handle != nullptr) {
                            TypeInfoHandle type_handle = static_cast<TypeInfoHandle>(
                                reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(raw_handle));
                            const auto* desc = TryDecodeReflectionQueryTypeHandle(type_handle);
                            if (desc != nullptr) {
                                clause.class_token = desc->metadata_token;
                            }
                        }
                    }
                }

                result.seh_clauses.push_back(clause);
                return true;
            });
    }

    // ── Step 4: Copy instructions to result ────────────────────────────
    result.instructions.reserve(raw_instructions.size());
    for (auto& instr : raw_instructions) {
        result.instructions.push_back(instr);
    }

    return result;
}

}  // namespace chaos::il2cpp::runtime_core