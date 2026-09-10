#include "interpreter_vm.h"

#include <cstring>

#include <memory_domain.h>

namespace chaos::il2cpp::interpreter {

// ── InterpreterValue lifecycle ──────────────────────────────────────────

void InterpreterValue::FreeStruct() {
    if (tag == ValueTag::Struct && obj != nullptr) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(obj);
        obj = nullptr;
        struct_size = 0u;
    }
}

InterpreterValue::InterpreterValue(const InterpreterValue& other) : tag(other.tag), struct_size(other.struct_size) {
    if (tag == ValueTag::Struct && other.obj != nullptr) {
        // Deep-copy struct data.
        obj = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(struct_size);
        if (obj != nullptr) {
            std::memcpy(obj, other.obj, struct_size);
        } else {
            struct_size = 0u;
        }
    } else if (tag == ValueTag::Int64) {
        i64 = other.i64;
    } else if (tag == ValueTag::Float64) {
        f64 = other.f64;
    } else {
        obj = other.obj;
    }
}

InterpreterValue& InterpreterValue::operator=(const InterpreterValue& other) {
    if (this == &other) {
        return *this;
    }

    // Free existing struct data before overwriting.
    FreeStruct();

    tag = other.tag;
    struct_size = other.struct_size;

    if (tag == ValueTag::Struct && other.obj != nullptr) {
        obj = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(struct_size);
        if (obj != nullptr) {
            std::memcpy(obj, other.obj, struct_size);
        } else {
            struct_size = 0u;
        }
    } else if (tag == ValueTag::Int64) {
        i64 = other.i64;
    } else if (tag == ValueTag::Float64) {
        f64 = other.f64;
    } else {
        obj = other.obj;
    }

    return *this;
}

InterpreterValue::~InterpreterValue() {
    FreeStruct();
}

// ── Factory methods ────────────────────────────────────────────────────

InterpreterValue InterpreterValue::from_i32(CHAOS_IL2CPP_INT32 value) {
    return InterpreterValue(value);
}

InterpreterValue InterpreterValue::from_i64(CHAOS_IL2CPP_INT64 value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Int64;
    result.i64 = value;
    return result;
}

InterpreterValue InterpreterValue::from_f32(float value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Float32;
    result.f32 = value;
    return result;
}

InterpreterValue InterpreterValue::from_f64(double value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Float64;
    result.f64 = value;
    return result;
}

InterpreterValue InterpreterValue::from_obj(void* value) {
    InterpreterValue result = {};
    result.tag = ValueTag::ObjectRef;
    result.obj = value;
    return result;
}

InterpreterValue InterpreterValue::from_struct(const void* data, CHAOS_IL2CPP_UINT32 size) {
    InterpreterValue result = {};
    result.tag = ValueTag::Struct;
    result.struct_size = size;
    if (data != nullptr && size > 0u) {
        result.obj = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(size);
        if (result.obj != nullptr) {
            std::memcpy(result.obj, data, size);
        } else {
            result.struct_size = 0u;
        }
    }
    return result;
}

InterpreterValue InterpreterValue::null_val() {
    InterpreterValue result = {};
    result.tag = ValueTag::Null;
    result.obj = nullptr;
    return result;
}

} // namespace chaos::il2cpp::interpreter
