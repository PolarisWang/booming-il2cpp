#include "code_buffer.h"

#include <chaos/pal/pal_mem.h>
#include <chaos/pal/pal_cache.h>

#include <cstdlib>

namespace chaos::il2cpp::jit {

CodeBuffer::CodeBuffer() noexcept {
    AllocPlatform(kCodeBufferInitSize);
}

CodeBuffer::~CodeBuffer() noexcept {
    FreePlatform();
}

CodeBuffer::CodeBuffer(CodeBuffer&& other) noexcept
    : data_(other.data_), pos_(other.pos_), capacity_(other.capacity_), alloc_size_(other.alloc_size_),
      failed_(other.failed_) {
    other.data_ = nullptr;
    other.pos_ = 0;
    other.capacity_ = 0;
    other.alloc_size_ = 0;
    other.failed_ = false;
}

CodeBuffer& CodeBuffer::operator=(CodeBuffer&& other) noexcept {
    if (this != &other) {
        FreePlatform();
        data_ = other.data_;
        pos_ = other.pos_;
        capacity_ = other.capacity_;
        alloc_size_ = other.alloc_size_;
        failed_ = other.failed_;
        other.data_ = nullptr;
        other.pos_ = 0;
        other.capacity_ = 0;
        other.alloc_size_ = 0;
        other.failed_ = false;
    }
    return *this;
}

void CodeBuffer::Ensure(uint32_t needed) noexcept {
    if (failed_)
        return;
    if (pos_ + needed > capacity_) {
        uint32_t new_cap = capacity_ * 2;
        if (new_cap < kCodeBufferInitSize)
            new_cap = kCodeBufferInitSize;
        if (new_cap > kCodeBufferMaxSize)
            new_cap = kCodeBufferMaxSize;
        if (pos_ + needed > new_cap) {
            new_cap = pos_ + needed + kCodeBufferInitSize;
            if (new_cap > kCodeBufferMaxSize) {
                CHAOS_IL2CPP_LOG_ERROR_M("codegen", "CodeBuffer: would exceed max size {}", kCodeBufferMaxSize);
                failed_ = true;
                return;
            }
        }
        if (!Grow(new_cap)) {
            failed_ = true;
        }
    }
}

bool CodeBuffer::Grow(uint32_t min_capacity) noexcept {
    uint32_t new_alloc = (min_capacity + 4095) & ~4095u; // Round up to page
    uint8_t* new_data = static_cast<uint8_t*>(chaos::il2cpp::pal::PalVirtualAlloc(new_alloc));

    if (new_data == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("codegen", "CodeBuffer: failed to allocate {} bytes", new_alloc);
        return false;
    }

    // Copy existing data
    if (pos_ > 0 && data_ != nullptr) {
        std::memcpy(new_data, data_, pos_);
    }

    // Free old allocation
    if (data_ != nullptr) {
        FreePlatform();
    }

    data_ = new_data;
    capacity_ = min_capacity;
    alloc_size_ = new_alloc;
    return true;
}

void* CodeBuffer::Seal() noexcept {
    if (data_ == nullptr || pos_ == 0 || failed_)
        return nullptr;

    // Trim to actual size (round up to page)
    uint32_t needed = (pos_ + 4095) & ~4095u;
    if (needed < alloc_size_) {
        // Free the excess pages
        chaos::il2cpp::pal::PalVirtualDecommit(static_cast<uint8_t*>(data_) + needed, alloc_size_ - needed);
    }

    if (!ProtectPlatform(true)) {
        return nullptr;
    }

    // Flush I-cache for the entire code range so ARM64 sees the newly written
    // instructions.  No-op on x86 (hardware I-cache coherency).
    chaos::il2cpp::pal::PalFlushInstructionCache(data_, pos_);

    // Transfer ownership to caller — reset internal state so ~CodeBuffer()
    // does not free the memory via FreePlatform().
    void* result = data_;
    data_ = nullptr;
    pos_ = 0;
    capacity_ = 0;
    alloc_size_ = 0;
    return result;
}

void CodeBuffer::FreePlatform() noexcept {
    if (data_ == nullptr)
        return;
    chaos::il2cpp::pal::PalVirtualFree(data_, alloc_size_);
    data_ = nullptr;
    capacity_ = 0;
    alloc_size_ = 0;
    pos_ = 0;
}

bool CodeBuffer::AllocPlatform(uint32_t size) noexcept {
    uint32_t alloc = (size + 4095) & ~4095u;
    data_ = static_cast<uint8_t*>(chaos::il2cpp::pal::PalVirtualAlloc(alloc));
    if (data_ == nullptr)
        return false;
    capacity_ = size;
    alloc_size_ = alloc;
    pos_ = 0;
    return true;
}

bool CodeBuffer::ProtectPlatform(bool executable) noexcept {
    if (data_ == nullptr)
        return false;
    auto prot = executable ? chaos::il2cpp::pal::kPalMemReadExec : chaos::il2cpp::pal::kPalMemReadWrite;
    return chaos::il2cpp::pal::PalVirtualProtect(data_, alloc_size_, prot);
}

} // namespace chaos::il2cpp::jit
