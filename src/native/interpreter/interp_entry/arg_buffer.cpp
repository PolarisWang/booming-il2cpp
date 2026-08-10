#include "interpreter_entry.h"

namespace chaos::il2cpp::runtime_core {

ArgBuffer::ArgBuffer(void* buffer) noexcept : buffer_(buffer) {}

CHAOS_IL2CPP_INT32 ArgBuffer::ReadI32() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_INT32*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_INT32);
    return value;
}

CHAOS_IL2CPP_INT64 ArgBuffer::ReadI64() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_INT64*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_INT64);
    return value;
}

CHAOS_IL2CPP_FLOAT32 ArgBuffer::ReadF32() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_FLOAT32*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT32);
    return value;
}

CHAOS_IL2CPP_FLOAT64 ArgBuffer::ReadF64() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_FLOAT64*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT64);
    return value;
}

void* ArgBuffer::ReadPtr() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<void**>(ptr);
    offset_ += sizeof(void*);
    return value;
}

void ArgBuffer::WriteI32(CHAOS_IL2CPP_INT32 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_INT32*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_INT32);
}

void ArgBuffer::WriteI64(CHAOS_IL2CPP_INT64 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_INT64*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_INT64);
}

void ArgBuffer::WriteF32(CHAOS_IL2CPP_FLOAT32 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_FLOAT32*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT32);
}

void ArgBuffer::WriteF64(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_FLOAT64*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT64);
}

void ArgBuffer::WritePtr(void* value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<void**>(ptr) = value;
    offset_ += sizeof(void*);
}

} // namespace chaos::il2cpp::runtime_core