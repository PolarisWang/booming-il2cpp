namespace chaos::il2cpp::runtime_core {

// ── ECMA element_type → ValueTag mapper ────────────────────────────────────
// Maps a single ECMA element type byte from a method signature to an
// interpreter::ValueTag. Returns ValueTag::ObjectRef for complex types
// (Class, ValueType, SzArray) that need pointer-based access.
static interpreter::ValueTag ElementTypeToValueTag(uint8_t elem_type) noexcept {
    using interpreter::ValueTag;
    switch (elem_type) {
    case 0x02: // Boolean
    case 0x03: // Char
    case 0x04: // I1 (SByte)
    case 0x05: // U1 (Byte)
    case 0x06: // I2 (Int16)
    case 0x07: // U2 (UInt16)
    case 0x08: // I4 (Int32)
    case 0x09: // U4 (UInt32)
        return ValueTag::Int32;
    case 0x0A: // I8 (Int64)
    case 0x0B: // U8 (UInt64)
    case 0x18: // I (IntPtr)
    case 0x19: // U (UIntPtr)
        return ValueTag::Int64;
    case 0x0C: // R4 (Single)
        return ValueTag::Float32;
    case 0x0D: // R8 (Double)
        return ValueTag::Float64;
    default:
        // 0x0E (String), 0x0F (Ptr), 0x10 (ByRef), 0x11 (ValueType),
        // 0x12 (Class), 0x1C (Object), 0x1D (SzArray) → pointer-sized
        return ValueTag::ObjectRef;
    }
}

// ── Read one argument from ArgBuffer with correct type ──────────────────────
// Reads from ArgBuffer using the type-appropriate read method and returns
// an InterpreterValue with the correct tag (not ObjectRef for primitives).
static interpreter::InterpreterValue ReadTypedArg(
    ArgBuffer& reader, interpreter::ValueTag tag) noexcept {
    switch (tag) {
    case interpreter::ValueTag::Int32:
        return interpreter::InterpreterValue::from_i32(reader.ReadI32());
    case interpreter::ValueTag::Int64:
        return interpreter::InterpreterValue::from_i64(reader.ReadI64());
    case interpreter::ValueTag::Float32:
        return interpreter::InterpreterValue::from_f32(reader.ReadF32());
    case interpreter::ValueTag::Float64:
        return interpreter::InterpreterValue::from_f64(reader.ReadF64());
    default:
        return interpreter::InterpreterValue::from_obj(reader.ReadPtr());
    }
}

// ── Write a return value to ret_buf with type-aware write ──────────────────
// Skips the tag switch when the return type is known.
static void WriteTypedRet(void* ret_buf, const interpreter::ExecutionResult& result,
                           interpreter::ValueTag ret_tag) noexcept {
    if (ret_buf == nullptr || !result.has_return_value) return;
    ArgBuffer ret_writer(ret_buf);
    switch (ret_tag) {
    case interpreter::ValueTag::Int32:
        ret_writer.WriteI32(result.return_value.i32);
        return;
    case interpreter::ValueTag::Int64:
        ret_writer.WriteI64(result.return_value.i64);
        return;
    case interpreter::ValueTag::Float32:
        ret_writer.WriteF32(result.return_value.f32);
        return;
    case interpreter::ValueTag::Float64:
        ret_writer.WriteF64(result.return_value.f64);
        return;
    default:
        break; // fall through to tag-switch
    }
    // Fallback: use tag from the return value itself.
    switch (result.return_value.tag) {
    case interpreter::ValueTag::Int32:
        ret_writer.WriteI32(result.return_value.i32); break;
    case interpreter::ValueTag::Int64:
        ret_writer.WriteI64(result.return_value.i64); break;
    case interpreter::ValueTag::Float32:
        ret_writer.WriteF32(result.return_value.f32); break;
    case interpreter::ValueTag::Float64:
        ret_writer.WriteF64(result.return_value.f64); break;
    default:
        ret_writer.WritePtr(result.return_value.obj); break;
    }
}

// ── Parse and cache method signature ───────────────────────────────────────
// Walks ECMA #Blob signature format, caches arg count, per-arg types,
// and return type in PatchMethod. Sets cached_sig_valid = true on success.
static void CacheSignature(PatchMethod* patch_method) noexcept {
    if (patch_method == nullptr) return;

    // ── Fast path: ECMA #Blob signature ─────────────────────────────────
    if (patch_method->signature_blob != nullptr && patch_method->signature_len > 1) {
        const uint8_t* sig = patch_method->signature_blob;
        uint32_t sig_len = patch_method->signature_len;

        // [blob_length] [calling_convention] [param_count] [ret_type] [param_types...]
        const uint8_t* ptr = sig + 1; // skip blob length
        uint32_t remaining = sig_len - 1;
        if (remaining < 2) return;

        uint8_t cc = ptr[0];
        uint8_t count_byte = ptr[1];

        uint32_t param_count = 0;
        uint32_t consumed = 2;
        if (count_byte <= 0x7F) {
            param_count = count_byte;
        } else if (count_byte <= 0xBF && remaining >= 3) {
            param_count = static_cast<uint32_t>(((count_byte & 0x3F) << 8) | ptr[2]);
            consumed = 3;
        } else {
            return; // 4-byte encoding, rare, skip
        }

        bool has_this = (cc & 0x20) == 0x20;
        uint32_t total_arg_count = has_this ? param_count + 1 : param_count;

        ptr += consumed;
        remaining -= consumed;
        if (remaining < 1) return;

        // Parse return type.
        patch_method->cached_ret_tag = static_cast<uint8_t>(
            ElementTypeToValueTag(ptr[0]));
        ptr += 1;
        remaining -= 1;

        // Parse parameter types.
        // Use small-buffer optimization: ≤8 args fits in cached_arg_types_small,
        // >8 args gets a heap allocation.
        uint8_t* arg_types_buf = nullptr;
        if (total_arg_count > 8) {
            arg_types_buf = CHAOS_IL2CPP_NEW_ARRAY(uint8_t, total_arg_count)();
            patch_method->cached_arg_types = arg_types_buf;
            patch_method->cached_arg_capacity = total_arg_count;
        }

        for (uint32_t i = 0; i < total_arg_count && remaining > 0; ++i) {
            uint8_t elem = ptr[0];
            auto tag = ElementTypeToValueTag(elem);
            patch_method->cached_arg_types[i] = static_cast<uint8_t>(tag);

            // Advance past this parameter type.
            ptr += 1;
            remaining -= 1;

            // For complex types with trailing token bytes, skip past them.
            if (elem == 0x11 || elem == 0x12) { // ValueType or Class
                // Skip TypeDefOrRef coded index (compressed unsigned int).
                if (remaining >= 1 && ptr[0] >= 0x80) {
                    if ((ptr[0] & 0xC0) == 0xC0 && remaining >= 4) {
                        ptr += 4; remaining -= 4;
                    } else if (remaining >= 2) {
                        ptr += 2; remaining -= 2;
                    }
                }
                // 1-byte tok: ptr already advanced by ptr+=1 above, any 0x7F or less is 1 byte
            }
        }

        patch_method->cached_arg_count = total_arg_count;
        patch_method->cached_sig_valid = true;
        return;
    }

    // ── Fallback: JIT methods (no signature blob) ─────────────────────────
    // When no ECMA #Blob signature is available (JIT mode), derive arg count
    // from the AOT core IR JSON. This enables interpreter dispatch for JIT
    // methods that lack signature_blob (RegisterJitMethods path).
    // All arg types default to ObjectRef (pointer-sized), which is safe for
    // the general case and prevents the null-pointer crash from arg_count=0.
    if (patch_method->aot_core_ir_json == nullptr) return;

    // Parse "parameterCount" from JSON: "...\"parameterCount\": N,..."
    const char* pc_marker = std::strstr(patch_method->aot_core_ir_json, "\"parameterCount\"");
    if (pc_marker == nullptr) return;

    const char* colon = std::strchr(pc_marker, ':');
    if (colon == nullptr) return;

    colon++;
    while (*colon == ' ' || *colon == '\t') colon++;

    char* end = nullptr;
    long count = std::strtol(colon, &end, 10);
    if (end == colon || count < 0) return;

    // Check "isStatic" to determine whether to add 'this' to arg count.
    bool is_static = false;
    const char* static_marker = std::strstr(patch_method->aot_core_ir_json, "\"isStatic\"");
    if (static_marker != nullptr) {
        const char* sv = std::strchr(static_marker, ':');
        if (sv != nullptr) {
            sv++;
            while (*sv == ' ' || *sv == '\t') sv++;
            if (*sv == 't' || *sv == '1') is_static = true;
        }
    }

    uint32_t total_args = is_static
        ? static_cast<uint32_t>(count)
        : static_cast<uint32_t>(count) + 1;

    // Allocate arg types if >8. For ≤8, cached_arg_types_small is used.
    if (total_args > 8) {
        auto* buf = CHAOS_IL2CPP_NEW_ARRAY(uint8_t, total_args)();
        patch_method->cached_arg_types = buf;
        patch_method->cached_arg_capacity = total_args;
    }

    // Default all arg types to ObjectRef (pointer-sized, safe for any ABI).
    // cached_ret_tag defaults to 0 (Void), which is handled by WriteTypedRet's
    // fallback path that reads the tag from the ExecutionResult itself.
    for (uint32_t i = 0; i < total_args; ++i) {
        patch_method->cached_arg_types[i] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
    }

    patch_method->cached_arg_count = total_args;
    patch_method->cached_sig_valid = true;
}

}  // namespace chaos::il2cpp::runtime_core
