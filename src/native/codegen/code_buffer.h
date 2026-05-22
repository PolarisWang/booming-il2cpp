#ifndef CHAOS_IL2CPP_CODEGEN_CODE_BUFFER_H_
#define CHAOS_IL2CPP_CODEGEN_CODE_BUFFER_H_

// ── CodeBuffer: RWX executable memory management ──────────────────────────
//
// Manages a growable buffer of executable memory allocated via VirtualAllex
// (Windows) or mmap (POSIX).  Used by the x64 code generator to emit native
// instructions that can be executed directly.
//
// Thread-safety: not thread-safe (single writer during code generation).
// After Seal(), the buffer is read-only and safe for concurrent execution.

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

#include <chaos/log.h>

namespace chaos::il2cpp::codegen {

// Default buffer size (4KB — fits most small-to-medium methods).
static constexpr uint32_t kCodeBufferInitSize = 4096;
// Max buffer size (64MB — safety limit for large generated methods).
static constexpr uint32_t kCodeBufferMaxSize  = 64u * 1024 * 1024;

class CodeBuffer {
public:
    CodeBuffer() noexcept;
    ~CodeBuffer() noexcept;

    // Non-copyable, movable.
    CodeBuffer(const CodeBuffer&) = delete;
    CodeBuffer& operator=(const CodeBuffer&) = delete;
    CodeBuffer(CodeBuffer&& other) noexcept;
    CodeBuffer& operator=(CodeBuffer&& other) noexcept;

    /// Emit a single byte.
    void EmitByte(uint8_t b) noexcept {
        if (failed_) return;
        Ensure(kMinGrow);
        data_[pos_++] = b;
    }

    /// Emit a 2-byte little-endian value.
    void Emit16(uint16_t v) noexcept {
        if (failed_) return;
        Ensure(2);
        data_[pos_++] = static_cast<uint8_t>(v);
        data_[pos_++] = static_cast<uint8_t>(v >> 8);
    }

    /// Emit a 4-byte little-endian value.
    void Emit32(uint32_t v) noexcept {
        if (failed_) return;
        Ensure(4);
        data_[pos_++] = static_cast<uint8_t>(v);
        data_[pos_++] = static_cast<uint8_t>(v >> 8);
        data_[pos_++] = static_cast<uint8_t>(v >> 16);
        data_[pos_++] = static_cast<uint8_t>(v >> 24);
    }

    /// Emit a 8-byte little-endian value.
    void Emit64(uint64_t v) noexcept {
        if (failed_) return;
        Ensure(8);
        data_[pos_++] = static_cast<uint8_t>(v);
        data_[pos_++] = static_cast<uint8_t>(v >> 8);
        data_[pos_++] = static_cast<uint8_t>(v >> 16);
        data_[pos_++] = static_cast<uint8_t>(v >> 24);
        data_[pos_++] = static_cast<uint8_t>(v >> 32);
        data_[pos_++] = static_cast<uint8_t>(v >> 40);
        data_[pos_++] = static_cast<uint8_t>(v >> 48);
        data_[pos_++] = static_cast<uint8_t>(v >> 56);
    }

    /// Emit a range of bytes.
    void EmitBytes(const void* src, uint32_t len) noexcept {
        if (failed_) return;
        Ensure(len);
        std::memcpy(data_ + pos_, src, len);
        pos_ += len;
    }

    /// Reserve space and return writable pointer (for fixups).
    uint8_t* Reserve(uint32_t len) noexcept {
        if (failed_) return nullptr;
        Ensure(len);
        uint8_t* p = data_ + pos_;
        pos_ += len;
        return p;
    }

    /// Patch a 4-byte value at a given offset (for branch fixups).
    void Patch32(uint32_t offset, uint32_t v) noexcept {
        if (offset + 4 > pos_) return;
        data_[offset + 0] = static_cast<uint8_t>(v);
        data_[offset + 1] = static_cast<uint8_t>(v >> 8);
        data_[offset + 2] = static_cast<uint8_t>(v >> 16);
        data_[offset + 3] = static_cast<uint8_t>(v >> 24);
    }

    /// Patch a 1-byte value at a given offset.
    void Patch8(uint32_t offset, uint8_t v) noexcept {
        if (offset < pos_) data_[offset] = v;
    }

    /// Seal the buffer: make it executable (RX) and return entry point.
    /// After sealing, Emit* must not be called.
    /// Returns pointer to executable code, or nullptr on failure (including OOM).
    void* Seal() noexcept;

    /// Returns true if an out-of-memory or alloc failure occurred.
    bool failed() const noexcept { return failed_; }

    /// Current write position (bytes emitted so far).
    uint32_t pos() const noexcept { return pos_; }

    /// Read a byte from the buffer at a given offset (for testing).
    /// Returns 0 if offset is past the current write position.
    uint8_t Peek(uint32_t offset) const noexcept {
        if (offset >= pos_) return 0;
        return data_[offset];
    }

    /// Get read-only pointer to the buffer contents (for testing).
    const uint8_t* Data() const noexcept { return data_; }

    /// Capacity of the underlying buffer.
    uint32_t capacity() const noexcept { return capacity_; }

    /// Reset write position (no reallocation, just rewind).
    void Reset() noexcept { pos_ = 0; }

private:
    static constexpr uint32_t kMinGrow = 16;

    void Ensure(uint32_t needed) noexcept;
    bool Grow(uint32_t min_capacity) noexcept;
    void FreePlatform() noexcept;
    bool AllocPlatform(uint32_t size) noexcept;
    bool ProtectPlatform(bool executable) noexcept;

    uint8_t*  data_     = nullptr;
    uint32_t  pos_      = 0;
    uint32_t  capacity_ = 0;
    uint32_t  alloc_size_ = 0;  // Actual allocation size (page-rounded)
    bool      failed_   = false;  // Set on allocation failure; suppresses further writes
};

}  // namespace chaos::il2cpp::codegen

#endif  // CHAOS_IL2CPP_CODEGEN_CODE_BUFFER_H_
