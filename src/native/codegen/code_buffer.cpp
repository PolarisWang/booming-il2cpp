#include "code_buffer.h"

#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
  #define NOMINMAX
  #include <windows.h>
#else
  #include <sys/mman.h>
  #include <unistd.h>
#endif

namespace chaos::il2cpp::codegen {

CodeBuffer::CodeBuffer() noexcept {
    AllocPlatform(kCodeBufferInitSize);
}

CodeBuffer::~CodeBuffer() noexcept {
    FreePlatform();
}

CodeBuffer::CodeBuffer(CodeBuffer&& other) noexcept
    : data_(other.data_), pos_(other.pos_), capacity_(other.capacity_), alloc_size_(other.alloc_size_) {
    other.data_ = nullptr;
    other.pos_ = 0;
    other.capacity_ = 0;
    other.alloc_size_ = 0;
}

CodeBuffer& CodeBuffer::operator=(CodeBuffer&& other) noexcept {
    if (this != &other) {
        FreePlatform();
        data_ = other.data_;
        pos_ = other.pos_;
        capacity_ = other.capacity_;
        alloc_size_ = other.alloc_size_;
        other.data_ = nullptr;
        other.pos_ = 0;
        other.capacity_ = 0;
        other.alloc_size_ = 0;
    }
    return *this;
}

void CodeBuffer::Ensure(uint32_t needed) noexcept {
    if (pos_ + needed > capacity_) {
        uint32_t new_cap = capacity_ * 2;
        if (new_cap < kCodeBufferInitSize) new_cap = kCodeBufferInitSize;
        if (new_cap > kCodeBufferMaxSize) new_cap = kCodeBufferMaxSize;
        if (pos_ + needed > new_cap) {
            new_cap = pos_ + needed + kCodeBufferInitSize;
            if (new_cap > kCodeBufferMaxSize) {
                CHAOS_IL2CPP_LOG_ERROR_M("codegen", "CodeBuffer: would exceed max size {}",
                                         kCodeBufferMaxSize);
                return;
            }
        }
        Grow(new_cap);
    }
}

bool CodeBuffer::Grow(uint32_t min_capacity) noexcept {
    uint32_t new_alloc = (min_capacity + 4095) & ~4095u;  // Round up to page
    uint8_t* new_data = nullptr;

#if defined(_WIN32) || defined(_WIN64)
    new_data = static_cast<uint8_t*>(
        VirtualAlloc(nullptr, new_alloc, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
#else
    new_data = static_cast<uint8_t*>(
        mmap(nullptr, new_alloc, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (new_data == MAP_FAILED) new_data = nullptr;
#endif

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
    if (data_ == nullptr || pos_ == 0) return nullptr;

    // Trim to actual size (round up to page)
    uint32_t needed = (pos_ + 4095) & ~4095u;
    if (needed < alloc_size_) {
        // Free the excess pages
#if defined(_WIN32) || defined(_WIN64)
        if (needed < alloc_size_) {
            VirtualFree(static_cast<uint8_t*>(data_) + needed,
                        alloc_size_ - needed, MEM_DECOMMIT);
        }
#endif
    }

    if (!ProtectPlatform(true)) {
        return nullptr;
    }

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
    if (data_ == nullptr) return;
#if defined(_WIN32) || defined(_WIN64)
    VirtualFree(data_, 0, MEM_RELEASE);
#else
    munmap(data_, alloc_size_);
#endif
    data_ = nullptr;
    capacity_ = 0;
    alloc_size_ = 0;
    pos_ = 0;
}

bool CodeBuffer::AllocPlatform(uint32_t size) noexcept {
    uint32_t alloc = (size + 4095) & ~4095u;
#if defined(_WIN32) || defined(_WIN64)
    data_ = static_cast<uint8_t*>(
        VirtualAlloc(nullptr, alloc, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
#else
    data_ = static_cast<uint8_t*>(
        mmap(nullptr, alloc, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (data_ == MAP_FAILED) data_ = nullptr;
#endif
    if (data_ == nullptr) return false;
    capacity_ = size;
    alloc_size_ = alloc;
    pos_ = 0;
    return true;
}

bool CodeBuffer::ProtectPlatform(bool executable) noexcept {
    if (data_ == nullptr) return false;
#if defined(_WIN32) || defined(_WIN64)
    DWORD old_protect;
    DWORD new_protect = executable ? PAGE_EXECUTE_READ : PAGE_READWRITE;
    return VirtualProtect(data_, alloc_size_, new_protect, &old_protect) != 0;
#else
    int prot = PROT_READ;
    if (executable) prot |= PROT_EXEC;
    return mprotect(data_, alloc_size_, prot) == 0;
#endif
}

}  // namespace chaos::il2cpp::codegen
