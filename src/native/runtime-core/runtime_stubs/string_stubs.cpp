// string_stubs.cpp — String helper stub implementations
#include <chaos/native_types.h>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── Internal helpers ──

/// Allocate a managed string (StubStringHeader + data + NUL) with the given byte_count.
/// Returns nullptr (0) on OOM. The caller fills data afterward.
static CHAOS_IL2CPP_INTPTR alloc_string(CHAOS_IL2CPP_UINTPTR byte_count) noexcept
{
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + byte_count + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = byte_count;
    reinterpret_cast<char*>(result + 1)[byte_count] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

/// ASCII-only in-place lowercasing (A-Z → a-z). Non-ASCII bytes pass through.
static void ascii_to_lower(char* data, CHAOS_IL2CPP_UINTPTR len) noexcept {
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        char c = data[i];
        if (c >= 'A' && c <= 'Z') data[i] = static_cast<char>(c + 32);
    }
}

/// ASCII-only in-place uppercasing (a-z → A-Z).
static void ascii_to_upper(char* data, CHAOS_IL2CPP_UINTPTR len) noexcept {
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < len; ++i) {
        char c = data[i];
        if (c >= 'a' && c <= 'z') data[i] = static_cast<char>(c - 32);
    }
}

inline static bool is_ascii_whitespace(char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    if (vh->byte_count == 0) return 1;

    const char* sdata = stub_string_data(reinterpret_cast<const void*>(str));
    const char* vdata = stub_string_data(reinterpret_cast<const void*>(value));
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= sh->byte_count - vh->byte_count; ++i) {
        if (std::memcmp(sdata + i, vdata, vh->byte_count) == 0) return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    return (std::memcmp(stub_string_data(reinterpret_cast<const void*>(str)),
                        stub_string_data(reinterpret_cast<const void*>(value)),
                        vh->byte_count) == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringEndsWith(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return 0;
    auto* sh = reinterpret_cast<const StubStringHeader*>(str);
    auto* vh = reinterpret_cast<const StubStringHeader*>(value);
    if (vh->byte_count > sh->byte_count) return 0;
    const char* sdata = stub_string_data(reinterpret_cast<const void*>(str));
    const char* vdata = stub_string_data(reinterpret_cast<const void*>(value));
    return (std::memcmp(sdata + sh->byte_count - vh->byte_count, vdata, vh->byte_count) == 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (separator == 0 || value == 0) return 0;
    auto* sep_hdr = reinterpret_cast<const StubStringHeader*>(separator);
    auto* arr_hdr = reinterpret_cast<StubArrayHeader*>(value);

    CHAOS_IL2CPP_UINTPTR count = arr_hdr->length;
    CHAOS_IL2CPP_UINTPTR sep_len = sep_hdr->byte_count;

    CHAOS_IL2CPP_UINTPTR total = 0;
    CHAOS_IL2CPP_INTPTR* elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<void*>(arr_hdr + 1));
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) {
        auto* elem = reinterpret_cast<const StubStringHeader*>(elements[i]);
        if (elem) total += elem->byte_count;
    }
    if (count > 1) total += sep_len * (count - 1);

    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + total + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = total;

    char* dest = reinterpret_cast<char*>(result + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < count; ++i) {
        if (i > 0 && sep_len > 0) {
            std::memcpy(dest, stub_string_data(reinterpret_cast<const void*>(separator)), sep_len);
            dest += sep_len;
        }
        auto* elem = reinterpret_cast<const StubStringHeader*>(elements[i]);
        if (elem && elem->byte_count > 0) {
            std::memcpy(dest, stub_string_data(reinterpret_cast<const void*>(elements[i])), elem->byte_count);
            dest += elem->byte_count;
        }
    }
    *dest = '\0';

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

// ── String stubs (real implementations) ──

CHAOS_IL2CPP_INT32 ChaosStringIndexOf(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (str == 0 || value == 0) return -1;
    const auto* str_hdr = reinterpret_cast<const StubStringHeader*>(str);
    const auto* val_hdr = reinterpret_cast<const StubStringHeader*>(value);
    if (val_hdr->byte_count > str_hdr->byte_count) return -1;
    if (val_hdr->byte_count == 0) return 0;
    const char* str_data = stub_string_data(reinterpret_cast<const void*>(str));
    const char* val_data = stub_string_data(reinterpret_cast<const void*>(value));
    CHAOS_IL2CPP_INT32 last = static_cast<CHAOS_IL2CPP_INT32>(str_hdr->byte_count - val_hdr->byte_count);
    for (CHAOS_IL2CPP_INT32 i = 0; i <= last; ++i) {
        if (std::memcmp(str_data + i, val_data, val_hdr->byte_count) == 0) return i;
    }
    return -1;
}

CHAOS_IL2CPP_INT32 ChaosStringCompare(CHAOS_IL2CPP_INTPTR str_a, CHAOS_IL2CPP_INTPTR str_b) noexcept
{
    if (str_a == 0 && str_b == 0) return 0;
    if (str_a == 0) return -1;
    if (str_b == 0) return 1;
    const auto* a_hdr = reinterpret_cast<const StubStringHeader*>(str_a);
    const auto* b_hdr = reinterpret_cast<const StubStringHeader*>(str_b);
    CHAOS_IL2CPP_INT32 min_len = a_hdr->byte_count < b_hdr->byte_count
        ? static_cast<CHAOS_IL2CPP_INT32>(a_hdr->byte_count)
        : static_cast<CHAOS_IL2CPP_INT32>(b_hdr->byte_count);
    const char* a_data = stub_string_data(reinterpret_cast<const void*>(str_a));
    const char* b_data = stub_string_data(reinterpret_cast<const void*>(str_b));
    int cmp = std::memcmp(a_data, b_data, static_cast<CHAOS_IL2CPP_SIZE>(min_len));
    if (cmp != 0) return cmp > 0 ? 1 : -1;
    if (a_hdr->byte_count == b_hdr->byte_count) return 0;
    return a_hdr->byte_count > b_hdr->byte_count ? 1 : -1;
}

CHAOS_IL2CPP_INTPTR ChaosStringSplit(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR separator) noexcept
{
    if (str == 0) return 0;
    const auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    const char* data = stub_string_data(reinterpret_cast<const void*>(str));
    char sep = static_cast<char>(separator & 0xFF);

    // Count parts
    CHAOS_IL2CPP_INT32 parts = 1;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < hdr->byte_count; ++i) {
        if (data[i] == sep) ++parts;
    }

    // Allocate result array
    CHAOS_IL2CPP_INTPTR result = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(GcAllocateAtomic(sizeof(StubArrayHeader) + static_cast<CHAOS_IL2CPP_SIZE>(parts) * sizeof(CHAOS_IL2CPP_INTPTR)));
    if (result == 0) return 0;
    auto* arr_hdr = reinterpret_cast<StubArrayHeader*>(result);
    arr_hdr->element_type = 0;
    arr_hdr->length = static_cast<CHAOS_IL2CPP_UINTPTR>(parts);
    auto* dest = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(arr_hdr + 1);

    // Scan and fill substrings
    CHAOS_IL2CPP_INT32 part_idx = 0;
    CHAOS_IL2CPP_UINTPTR seg_start = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= hdr->byte_count; ++i) {
        if (i == hdr->byte_count || data[i] == sep) {
            CHAOS_IL2CPP_UINTPTR seg_len = i - seg_start;
            dest[part_idx] = alloc_string(seg_len);
            if (dest[part_idx] != 0 && seg_len > 0) {
                char* seg_dst = reinterpret_cast<char*>(reinterpret_cast<StubStringHeader*>(dest[part_idx]) + 1);
                std::memcpy(seg_dst, data + seg_start, seg_len);
            }
            ++part_idx;
            seg_start = i + 1;
        }
    }
    return result;
}

CHAOS_IL2CPP_INTPTR ChaosStringFastAllocate(CHAOS_IL2CPP_INTPTR length) noexcept
{
    if (length < 0) return 0;
    return alloc_string(static_cast<CHAOS_IL2CPP_UINTPTR>(length));
}

CHAOS_IL2CPP_INTPTR ChaosStringSubstring(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept
{
    if (str == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    auto si = static_cast<CHAOS_IL2CPP_UINTPTR>(start_index);
    auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(length);
    if (si > hdr->byte_count || len > hdr->byte_count - si) return 0;
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + len + 1));
    if (result == nullptr) return 0;
    result->type = 0;
    result->byte_count = len;
    const char* src = stub_string_data(reinterpret_cast<const void*>(str));
    std::memcpy(result + 1, src + si, len);
    reinterpret_cast<char*>(result + 1)[len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringRemove(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR start_index, CHAOS_IL2CPP_INTPTR length) noexcept
{
    if (str == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    auto si = static_cast<CHAOS_IL2CPP_UINTPTR>(start_index);
    auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(length);
    if (si > hdr->byte_count || len > hdr->byte_count - si) return 0;
    CHAOS_IL2CPP_UINTPTR new_len = hdr->byte_count - len;
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + new_len + 1));
    if (result == nullptr) return 0;
    result->byte_count = new_len;
    const char* src = stub_string_data(reinterpret_cast<const void*>(str));
    char* dst = reinterpret_cast<char*>(result + 1);
    std::memcpy(dst, src, si);
    std::memcpy(dst + si, src + si + len, new_len - si);
    dst[new_len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringToLower(CHAOS_IL2CPP_INTPTR str) noexcept
{
    if (str == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + hdr->byte_count + 1));
    if (result == nullptr) return 0;
    result->byte_count = hdr->byte_count;
    const char* src = stub_string_data(reinterpret_cast<const void*>(str));
    char* dst = reinterpret_cast<char*>(result + 1);
    std::memcpy(dst, src, hdr->byte_count);
    ascii_to_lower(dst, hdr->byte_count);
    dst[hdr->byte_count] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringToUpper(CHAOS_IL2CPP_INTPTR str) noexcept
{
    if (str == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + hdr->byte_count + 1));
    if (result == nullptr) return 0;
    result->byte_count = hdr->byte_count;
    const char* src = stub_string_data(reinterpret_cast<const void*>(str));
    char* dst = reinterpret_cast<char*>(result + 1);
    std::memcpy(dst, src, hdr->byte_count);
    ascii_to_upper(dst, hdr->byte_count);
    dst[hdr->byte_count] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringTrim(CHAOS_IL2CPP_INTPTR str) noexcept
{
    if (str == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    const char* data = stub_string_data(reinterpret_cast<const void*>(str));
    CHAOS_IL2CPP_UINTPTR len = hdr->byte_count;
    CHAOS_IL2CPP_UINTPTR start = 0;
    while (start < len && is_ascii_whitespace(data[start])) ++start;
    CHAOS_IL2CPP_UINTPTR end = len;
    while (end > start && is_ascii_whitespace(data[end - 1])) --end;
    CHAOS_IL2CPP_UINTPTR new_len = end - start;
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + new_len + 1));
    if (result == nullptr) return 0;
    result->byte_count = new_len;
    if (new_len > 0) std::memcpy(result + 1, data + start, new_len);
    reinterpret_cast<char*>(result + 1)[new_len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringReplace(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR old_value, CHAOS_IL2CPP_INTPTR new_value) noexcept
{
    if (str == 0 || old_value == 0 || new_value == 0) return 0;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    auto* old_hdr = reinterpret_cast<const StubStringHeader*>(old_value);
    auto* new_hdr = reinterpret_cast<const StubStringHeader*>(new_value);
    const char* sdata = stub_string_data(reinterpret_cast<const void*>(str));
    const char* odata = stub_string_data(reinterpret_cast<const void*>(old_value));
    const char* ndata = stub_string_data(reinterpret_cast<const void*>(new_value));

    if (old_hdr->byte_count == 0) {
        // Nothing to replace — return a copy.
        auto* result = static_cast<StubStringHeader*>(
            GcAllocateAtomic(sizeof(StubStringHeader) + hdr->byte_count + 1));
        if (result == nullptr) return 0;
        result->byte_count = hdr->byte_count;
        std::memcpy(result + 1, sdata, hdr->byte_count);
        reinterpret_cast<char*>(result + 1)[hdr->byte_count] = '\0';
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
    }

    // Count occurrences
    CHAOS_IL2CPP_UINTPTR count = 0;
    for (CHAOS_IL2CPP_UINTPTR i = 0; i <= hdr->byte_count - old_hdr->byte_count; ) {
        if (std::memcmp(sdata + i, odata, old_hdr->byte_count) == 0) {
            ++count;
            i += old_hdr->byte_count;
        } else {
            ++i;
        }
    }

    CHAOS_IL2CPP_UINTPTR new_len = hdr->byte_count
        + count * (new_hdr->byte_count - old_hdr->byte_count);
    auto* result = static_cast<StubStringHeader*>(
        GcAllocateAtomic(sizeof(StubStringHeader) + new_len + 1));
    if (result == nullptr) return 0;
    result->byte_count = new_len;
    char* dst = reinterpret_cast<char*>(result + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < hdr->byte_count; ) {
        if (i <= hdr->byte_count - old_hdr->byte_count &&
            std::memcmp(sdata + i, odata, old_hdr->byte_count) == 0) {
            std::memcpy(dst, ndata, new_hdr->byte_count);
            dst += new_hdr->byte_count;
            i += old_hdr->byte_count;
        } else {
            *dst++ = sdata[i++];
        }
    }
    *dst = '\0';

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringFormatArray(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR args_array) noexcept
{
	if (format_str == 0) return 0;

	auto* fhdr = reinterpret_cast<const StubStringHeader*>(format_str);
	const char* fmt = stub_string_data(reinterpret_cast<const void*>(format_str));
	CHAOS_IL2CPP_UINTPTR fmt_len = fhdr->byte_count;

	// Get arg count and elements from the array header (if present)
	CHAOS_IL2CPP_UINTPTR arg_count = 0;
	CHAOS_IL2CPP_INTPTR* elements = nullptr;
	if (args_array != 0) {
		auto* arr_hdr = reinterpret_cast<const StubArrayHeader*>(args_array);
		arg_count = arr_hdr->length;
		elements = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
			const_cast<StubArrayHeader*>(arr_hdr) + 1);
	}

	// Helper: parse a {N} placeholder and return the element index, or ~0 on failure.
	auto parse_placeholder = [&](CHAOS_IL2CPP_UINTPTR pos, CHAOS_IL2CPP_UINTPTR& end) -> CHAOS_IL2CPP_UINTPTR {
		if (pos >= fmt_len || fmt[pos] != '{') return ~CHAOS_IL2CPP_UINTPTR(0);
		CHAOS_IL2CPP_UINTPTR scan = pos + 1;
		if (scan >= fmt_len || fmt[scan] == '}') return ~CHAOS_IL2CPP_UINTPTR(0);
		CHAOS_IL2CPP_UINTPTR idx = 0;
		bool any_digit = false;
		while (scan < fmt_len && fmt[scan] >= '0' && fmt[scan] <= '9') {
			idx = idx * 10 + static_cast<CHAOS_IL2CPP_UINTPTR>(fmt[scan] - '0');
			++scan;
			any_digit = true;
		}
		if (!any_digit || scan >= fmt_len || fmt[scan] != '}') return ~CHAOS_IL2CPP_UINTPTR(0);
		end = scan + 1;
		return idx;
	};

	// First pass: calculate output length
	CHAOS_IL2CPP_UINTPTR out_len = 0;
	for (CHAOS_IL2CPP_UINTPTR i = 0; i < fmt_len; ) {
		if (fmt[i] == '{' && i + 1 < fmt_len && fmt[i + 1] == '{') {
			out_len += 1;
			i += 2;
		} else if (fmt[i] == '}' && i + 1 < fmt_len && fmt[i + 1] == '}') {
			out_len += 1;
			i += 2;
		} else if (fmt[i] == '{') {
			CHAOS_IL2CPP_UINTPTR end = 0;
			CHAOS_IL2CPP_UINTPTR idx = parse_placeholder(i, end);
			if (idx != ~CHAOS_IL2CPP_UINTPTR(0) && idx < arg_count) {
				auto* elem = reinterpret_cast<const StubStringHeader*>(elements[idx]);
				out_len += (elem != nullptr) ? elem->byte_count : 6;
				i = end;
			} else {
				++out_len;
				++i;
			}
		} else {
			++out_len;
			++i;
		}
	}

	auto* result = static_cast<StubStringHeader*>(
		GcAllocateAtomic(sizeof(StubStringHeader) + out_len + 1));
	if (result == nullptr) return 0;
	result->type = 0;
	result->byte_count = out_len;
	char* dst = reinterpret_cast<char*>(result + 1);

	// Second pass: build output
	for (CHAOS_IL2CPP_UINTPTR i = 0; i < fmt_len; ) {
		if (fmt[i] == '{' && i + 1 < fmt_len && fmt[i + 1] == '{') {
			*dst++ = '{';
			i += 2;
		} else if (fmt[i] == '}' && i + 1 < fmt_len && fmt[i + 1] == '}') {
			*dst++ = '}';
			i += 2;
		} else if (fmt[i] == '{') {
			CHAOS_IL2CPP_UINTPTR end = 0;
			CHAOS_IL2CPP_UINTPTR idx = parse_placeholder(i, end);
			if (idx != ~CHAOS_IL2CPP_UINTPTR(0) && idx < arg_count) {
				auto* elem = reinterpret_cast<const StubStringHeader*>(elements[idx]);
				if (elem && elem->byte_count > 0) {
					std::memcpy(dst, stub_string_data(elem), elem->byte_count);
					dst += elem->byte_count;
				} else if (elem == nullptr) {
					std::memcpy(dst, "(null)", 6);
					dst += 6;
				}
				i = end;
			} else {
				*dst++ = fmt[i++];
			}
		} else {
			*dst++ = fmt[i++];
		}
	}
	*dst = '\0';
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR ChaosStringFormat1(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR arg0) noexcept
{
	struct { StubArrayHeader hdr; CHAOS_IL2CPP_INTPTR data[1]; } local;
	local.hdr.element_type = 0;
	local.hdr.length = 1;
	local.data[0] = arg0;
	return ChaosStringFormatArray(format_str,
		reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&local));
}

CHAOS_IL2CPP_INTPTR ChaosStringFormat2(CHAOS_IL2CPP_INTPTR format_str, CHAOS_IL2CPP_INTPTR arg0, CHAOS_IL2CPP_INTPTR arg1) noexcept
{
	struct { StubArrayHeader hdr; CHAOS_IL2CPP_INTPTR data[2]; } local;
	local.hdr.element_type = 0;
	local.hdr.length = 2;
	local.data[0] = arg0;
	local.data[1] = arg1;
	return ChaosStringFormatArray(format_str,
		reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&local));
}

void ChaosStringAppend(CHAOS_IL2CPP_INTPTR builder, CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR arg) noexcept
{
    if (builder == 0 || str == 0) return;
    auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    auto* buf_hdr = reinterpret_cast<StubStringHeader*>(builder);
    CHAOS_IL2CPP_UINTPTR old_len = buf_hdr->byte_count;
    CHAOS_IL2CPP_UINTPTR append_len = hdr->byte_count;
    // Append str's data to builder's data buffer
    const char* src = stub_string_data(reinterpret_cast<const void*>(str));
    char* dst = reinterpret_cast<char*>(buf_hdr + 1);
    std::memcpy(dst + old_len, src, append_len);
    buf_hdr->byte_count = old_len + append_len;
    dst[old_len + append_len] = '\0';
    (void)arg;
}

CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR format, CHAOS_IL2CPP_INTPTR args) noexcept
{
    // V1: just return the format string unchanged
    (void)args;
    return format;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core