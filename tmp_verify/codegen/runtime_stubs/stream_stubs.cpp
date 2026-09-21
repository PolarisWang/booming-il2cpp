// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// stream_stubs.cpp — Stream/IO stub implementations
#include <chaos/native_types.h>
#include <chaos/log.h>
#include <cstring>
#include <cstdint>
#include <vector>
#include <string>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "string_table.h"
#include "gc_helpers.h"
#include "runtime_stubs/stream_state.h"

namespace chaos::il2cpp::runtime_core {

// ── Global side map definition ─────────────────────────────────
CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, StreamState) g_stream_state;
std::mutex g_stream_state_mutex;

// ── Internal helpers ───────────────────────────────────────────

/// Managed string header size: ThinLockableHeader(16) + length(4) +
/// padding(4) + utf8_data(8) + string_id(8) = 40 bytes on x64.
static constexpr CHAOS_IL2CPP_SIZE kManagedStringHeader = 40;

/// Allocate a managed string with the given byte_count of UTF-8 payload + NUL.
static CHAOS_IL2CPP_INTPTR alloc_string(const char* data, size_t len) noexcept {
    auto* storage = static_cast<uint8_t*>(GcAllocateAtomic(kManagedStringHeader + len + 1));
    if (!storage) return 0;
    std::memset(storage, 0, kManagedStringHeader);
    *reinterpret_cast<int32_t*>(storage + 16) = static_cast<int32_t>(len);
    char* data_area = reinterpret_cast<char*>(storage + kManagedStringHeader);
    if (len > 0) std::memcpy(data_area, data, len);
    data_area[len] = '\0';
    *reinterpret_cast<const char**>(storage + 24) = data_area;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

/// Read a managed string's content into a C++ string.
/// Handles both raw managed object pointers and compile-time string IDs
/// (CHAOS_IL2CPP_STRING_ID constants from AOT codegen).
static std::string read_managed_string(CHAOS_IL2CPP_INTPTR str_handle) noexcept {
    if (str_handle == 0) return {};

    // Handle compile-time tagged string IDs (from CHAOS_IL2CPP_STRING_ID)
    if (chaos_is_string_id(str_handle)) {
        auto id = chaos_extract_string_id(str_handle);
        auto sv = chaos::il2cpp::string_table::Resolve(id);
        if (sv.utf8_data != nullptr) {
            return std::string(sv.utf8_data, sv.byte_count);
        }
        return {};
    }

    // Raw managed object pointer path
    int32_t len = *reinterpret_cast<const int32_t*>(str_handle + 16);
    const char* data = *reinterpret_cast<const char* const*>(str_handle + 24);
    return std::string(data, static_cast<size_t>(len > 0 ? len : 0));
}

/// Read a managed byte[] into a vector. Uses chaos_managed_array layout:
///   header(16) + element_type_shape(1) + padding(7) + element_type_info(8) + length(8) + data[]
/// Length at offset 32, data starts at offset 40.
static constexpr CHAOS_IL2CPP_SIZE kManagedArrayLengthOffset = 32;
static constexpr CHAOS_IL2CPP_SIZE kManagedArrayDataOffset = 40;
static std::vector<uint8_t> read_managed_byte_array(CHAOS_IL2CPP_INTPTR arr_handle) noexcept {
    if (arr_handle == 0) return {};
    int64_t len = *reinterpret_cast<const int64_t*>(arr_handle + kManagedArrayLengthOffset);
    if (len <= 0) return {};
    const uint8_t* data = reinterpret_cast<const uint8_t*>(arr_handle + kManagedArrayDataOffset);
    return std::vector<uint8_t>(data, data + len);
}

/// Get or create stream state for a managed object pointer.
static StreamState& get_or_create_state(void* key) noexcept {
    StreamStateLock lock;
    return g_stream_state[key];
}

/// Find stream state for a managed object pointer. Returns nullptr if not found.
static StreamState* find_state(void* key) noexcept {
    StreamStateLock lock;
    auto it = g_stream_state.find(key);
    return it != g_stream_state.end() ? &it->second : nullptr;
}

/// Remove stream state.
static void remove_state(void* key) noexcept {
    StreamStateLock lock;
    g_stream_state.erase(key);
}

extern "C" {

// ── MemoryStream constructors ──────────────────────────────────

void ChaosMemoryStreamCtor(CHAOS_IL2CPP_INTPTR instance) noexcept {
    // Parameterless MemoryStream: empty buffer
    auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
    state.kind = StreamKind::MemoryStream;
    state.buffer.clear();
    state.position = 0;
}

void ChaosMemoryStreamCtorWithBuffer(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR buffer) noexcept {
    // MemoryStream(byte[]): copy buffer data into our side map
    auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
    state.kind = StreamKind::MemoryStream;
    state.buffer = read_managed_byte_array(buffer);
    state.position = 0;
}

// ── TextReader/StringReader constructors ───────────────────────

void ChaosStringReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR input) noexcept {
    auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
    state.kind = StreamKind::StringReader;
    state.text = read_managed_string(input);
    state.read_pos = 0;
}

// ── TextWriter/StringWriter constructors ───────────────────────

void ChaosStringWriterCtor(CHAOS_IL2CPP_INTPTR instance) noexcept {
    auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
    state.kind = StreamKind::StringWriter;
    state.accumulated.clear();
}

// ── Stream methods (MemoryStream) ──────────────────────────────

void ChaosStreamFlush(CHAOS_IL2CPP_INTPTR stream) noexcept {
    (void)stream;
    // MemoryStream.Flush is a documented no-op in .NET
}

CHAOS_IL2CPP_INT64 ChaosStreamSeek(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 offset, CHAOS_IL2CPP_INT32 origin) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(stream));
    if (!state || state->kind != StreamKind::MemoryStream) return 0;

    int64_t new_pos = state->position;
    int64_t len = static_cast<int64_t>(state->buffer.size());

    switch (origin) {
        case 0: new_pos = offset; break;                              // SeekOrigin.Begin
        case 1: new_pos = state->position + offset; break;            // SeekOrigin.Current
        case 2: new_pos = len + offset; break;                        // SeekOrigin.End
        default: return 0;
    }
    if (new_pos < 0) new_pos = 0;
    state->position = new_pos;
    return new_pos;
}

void ChaosStreamSetLength(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 value) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(stream));
    if (!state || state->kind != StreamKind::MemoryStream) return;

    auto new_len = static_cast<size_t>(value > 0 ? value : 0);
    state->buffer.resize(new_len);
    if (state->position > static_cast<int64_t>(new_len))
        state->position = static_cast<int64_t>(new_len);
}

CHAOS_IL2CPP_INT64 ChaosStreamGetLength(CHAOS_IL2CPP_INTPTR stream) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(stream));
    if (!state || state->kind != StreamKind::MemoryStream) return 0;
    return static_cast<CHAOS_IL2CPP_INT64>(state->buffer.size());
}

CHAOS_IL2CPP_INT64 ChaosStreamGetPosition(CHAOS_IL2CPP_INTPTR stream) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(stream));
    if (!state || state->kind != StreamKind::MemoryStream) return 0;
    return state->position;
}

CHAOS_IL2CPP_INT32 ChaosStreamRead(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(stream));
    if (!state || state->kind != StreamKind::MemoryStream) return 0;

    auto avail = static_cast<int64_t>(state->buffer.size()) - state->position;
    if (avail <= 0) return 0;
    int64_t to_read = count < avail ? static_cast<int64_t>(count) : avail;
    if (to_read <= 0) return 0;

    // Write into managed byte[] at offset 40 (after chaos_managed_array header)
    auto* dest = reinterpret_cast<uint8_t*>(buffer) + kManagedArrayDataOffset + offset;
    std::memcpy(dest, state->buffer.data() + state->position, static_cast<size_t>(to_read));
    state->position += to_read;
    return static_cast<CHAOS_IL2CPP_INT32>(to_read);
}

void ChaosStreamWrite(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(stream));
    if (!state || state->kind != StreamKind::MemoryStream) return;
    if (count <= 0) return;

    // Read from managed byte[] at offset 40 (after chaos_managed_array header)
    auto* src = reinterpret_cast<const uint8_t*>(buffer) + kManagedArrayDataOffset + offset;
    auto pos = static_cast<size_t>(state->position);
    auto cnt = static_cast<size_t>(count);

    if (pos + cnt > state->buffer.size())
        state->buffer.resize(pos + cnt);

    std::memcpy(state->buffer.data() + pos, src, cnt);
    state->position = static_cast<int64_t>(pos + cnt);
}

void ChaosStreamCopyTo(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR destination) noexcept {
    auto* src_state = find_state(reinterpret_cast<void*>(source));
    auto* dst_state = find_state(reinterpret_cast<void*>(destination));
    if (!src_state || !dst_state) return;
    if (src_state->kind != StreamKind::MemoryStream || dst_state->kind != StreamKind::MemoryStream) return;

    auto pos = static_cast<size_t>(src_state->position);
    if (pos >= src_state->buffer.size()) return;

    // Append remaining source data to destination buffer
    auto remaining = src_state->buffer.size() - pos;
    size_t old_dst_size = dst_state->buffer.size();
    dst_state->buffer.resize(old_dst_size + remaining);
    std::memcpy(dst_state->buffer.data() + old_dst_size, src_state->buffer.data() + pos, remaining);
    src_state->position = static_cast<int64_t>(src_state->buffer.size());
}

// ── TextReader methods ─────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosTextReaderReadLine(CHAOS_IL2CPP_INTPTR reader) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(reader));
    if (!state || state->kind != StreamKind::StringReader) {
        // Return empty string for invalid state
        return alloc_string("", 0);
    }

    auto pos = static_cast<size_t>(state->read_pos);
    if (pos >= state->text.size()) {
        return alloc_string("", 0);
    }

    // Scan for newline
    auto newline_pos = state->text.find('\n', pos);
    size_t line_end;
    if (newline_pos == std::string::npos) {
        line_end = state->text.size();
    } else {
        line_end = newline_pos;
    }

    auto line_len = line_end - pos;
    auto result = alloc_string(state->text.data() + pos, line_len);
    state->read_pos = static_cast<int64_t>(newline_pos != std::string::npos ? newline_pos + 1 : state->text.size());
    return result;
}

CHAOS_IL2CPP_INTPTR ChaosTextReaderReadToEnd(CHAOS_IL2CPP_INTPTR reader) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(reader));
    if (!state || state->kind != StreamKind::StringReader) {
        return alloc_string("", 0);
    }

    auto pos = static_cast<size_t>(state->read_pos);
    if (pos >= state->text.size()) {
        return alloc_string("", 0);
    }

    auto remaining = state->text.size() - pos;
    auto result = alloc_string(state->text.data() + pos, remaining);
    state->read_pos = static_cast<int64_t>(state->text.size());
    return result;
}

// ── TextWriter methods ─────────────────────────────────────────

void ChaosTextWriterWrite(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(writer));
    if (!state || state->kind != StreamKind::StringWriter) return;
    state->accumulated += read_managed_string(value);
}

void ChaosTextWriterWriteLine(CHAOS_IL2CPP_INTPTR writer) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(writer));
    if (!state || state->kind != StreamKind::StringWriter) return;
    state->accumulated += '\n';
}

void ChaosTextWriterWriteLineStr(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(writer));
    if (!state || state->kind != StreamKind::StringWriter) return;
    state->accumulated += read_managed_string(value);
    state->accumulated += '\n';
}

// ── BinaryReader ───────────────────────────────────────────────

/// BinaryReader ctor: store the underlying stream pointer in our side map.
void ChaosBinaryReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR stream) noexcept {
    auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
    state.kind = StreamKind::None;
    // Store the stream pointer — reused by Read methods.
    // We store it in the buffer's pointer representation.
    state.buffer.clear();
    state.position = static_cast<int64_t>(stream);
}

CHAOS_IL2CPP_INT32 ChaosBinaryReaderReadInt32(CHAOS_IL2CPP_INTPTR reader) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(reader));
    if (!state) return 0;

    auto* stream = reinterpret_cast<void*>(state->position);
    auto* s = find_state(stream);
    if (!s || s->kind != StreamKind::MemoryStream) return 0;

    auto pos = static_cast<size_t>(s->position);
    if (pos + 4 > s->buffer.size()) return 0;

    int32_t value;
    std::memcpy(&value, s->buffer.data() + pos, sizeof(value));
    s->position = static_cast<int64_t>(pos + 4);
    return value;
}

CHAOS_IL2CPP_INTPTR ChaosBinaryReaderReadString(CHAOS_IL2CPP_INTPTR reader) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(reader));
    if (!state) return alloc_string("", 0);

    auto* stream = reinterpret_cast<void*>(state->position);
    auto* s = find_state(stream);
    if (!s || s->kind != StreamKind::MemoryStream) return alloc_string("", 0);

    auto pos = static_cast<size_t>(s->position);
    if (pos >= s->buffer.size()) return alloc_string("", 0);

    // 7-bit encoded int32 length
    uint8_t b;
    uint32_t str_len = 0;
    int shift = 0;
    do {
        if (pos >= s->buffer.size()) return alloc_string("", 0);
        b = s->buffer[pos++];
        str_len |= static_cast<uint32_t>(b & 0x7F) << shift;
        shift += 7;
    } while ((b & 0x80) != 0);

    if (pos + str_len > s->buffer.size()) return alloc_string("", 0);
    auto result = alloc_string(reinterpret_cast<const char*>(s->buffer.data() + pos), str_len);
    s->position = static_cast<int64_t>(pos + str_len);
    return result;
}

double ChaosBinaryReaderReadDouble(CHAOS_IL2CPP_INTPTR reader) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(reader));
    if (!state) return 0.0;

    auto* stream = reinterpret_cast<void*>(state->position);
    auto* s = find_state(stream);
    if (!s || s->kind != StreamKind::MemoryStream) return 0.0;

    auto pos = static_cast<size_t>(s->position);
    if (pos + 8 > s->buffer.size()) return 0.0;

    double value;
    std::memcpy(&value, s->buffer.data() + pos, sizeof(value));
    s->position = static_cast<int64_t>(pos + 8);
    return value;
}

// ── BinaryWriter ───────────────────────────────────────────────

/// BinaryWriter ctor: store the underlying stream pointer in our side map.
void ChaosBinaryWriterCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR stream) noexcept {
    auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
    state.kind = StreamKind::None;
    state.buffer.clear();
    state.position = static_cast<int64_t>(stream);
}

void ChaosBinaryWriterWriteInt32(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INT32 value) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(writer));
    if (!state) return;

    auto* stream = reinterpret_cast<void*>(state->position);
    auto* s = find_state(stream);
    if (!s || s->kind != StreamKind::MemoryStream) return;

    auto pos = static_cast<size_t>(s->position);
    if (pos + 4 > s->buffer.size())
        s->buffer.resize(pos + 4);
    std::memcpy(s->buffer.data() + pos, &value, sizeof(value));
    s->position = static_cast<int64_t>(pos + 4);
}

void ChaosBinaryWriterWriteString(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept {
    auto* state = find_state(reinterpret_cast<void*>(writer));
    if (!state) return;

    auto* stream = reinterpret_cast<void*>(state->position);
    auto* s = find_state(stream);
    if (!s || s->kind != StreamKind::MemoryStream) return;

    auto str = read_managed_string(value);
    auto byte_count = str.size();

    // 7-bit encoded length
    auto pos = static_cast<size_t>(s->position);
    uint32_t len32 = static_cast<uint32_t>(byte_count);
    size_t header_size = 1;
    while (len32 > 0x7F) { len32 >>= 7; header_size++; }
    len32 = static_cast<uint32_t>(byte_count);

    s->buffer.resize(pos + header_size + byte_count);
    uint32_t tmp = len32;
    do {
        uint8_t b = static_cast<uint8_t>(tmp & 0x7F);
        tmp >>= 7;
        if (tmp != 0) b |= 0x80;
        s->buffer[pos++] = b;
    } while (tmp != 0);

    if (byte_count > 0)
        std::memcpy(s->buffer.data() + pos, str.data(), byte_count);
    s->position = static_cast<int64_t>(pos + byte_count);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
