// xml_writer_async_stubs.cpp — native AOT implementation of System.Xml.XmlWriter's
// *Async surface, for the ATG fixture writer (non-async-capable).
//
// Contract derivation
// -------------------
// ATG synthesises these subjects against `XmlWriter.Create(new StringBuilder())`.
// That writer reports `Settings.Async == false`, and the BCL's Write*Async family
// refuses to run on a writer that is not async-capable.  Measured against .NET 8,
// the resulting behaviour is uniform once the *argument* checks pass:
//
//     fresh writer, WriteStartDocumentAsync()      -> InvalidOperationException
//     fresh writer, WriteEndDocumentAsync()        -> InvalidOperationException
//     fresh writer, FlushAsync()                   -> InvalidOperationException
//     fresh writer, WriteStringAsync("x")          -> InvalidOperationException
//     ... every valid-argument call                -> InvalidOperationException
//
// while argument validation still runs FIRST and keeps its own exception:
//
//     WriteNameAsync(null)                         -> ArgumentNullException
//     WriteNmTokenAsync(null)                      -> ArgumentNullException
//     WriteEntityRefAsync(null)                    -> ArgumentNullException
//     WriteDocTypeAsync(null, ...)                 -> ArgumentNullException
//     WriteQualifiedNameAsync(null, null)          -> ArgumentNullException
//     WriteStartElementAsync(null, null, null)     -> ArgumentNullException
//     WriteCharEntityAsync(<surrogate>)            -> ArgumentException
//     WriteSurrogateCharEntityAsync('a','b')       -> ArgumentException
//
// and a couple of arguments that are legitimately nullable stay non-throwing:
//
//     WriteStringAsync(null)                       -> OK
//     WriteRawAsync(null)                          -> OK
//
// Hence each entry below validates its arguments the way the *sync* path does
// (that validation is shared by both paths in the BCL), then raises
// InvalidOperationException on the otherwise-valid path.  The sync entries in
// xml_writer_stubs.cpp must NOT be reused directly: they succeed — or are
// idempotent, e.g. ChaosXmlWriterWriteStartDocument returns early when the
// document was already started — where the async path must raise.
//
// Handle handling
// ---------------
// The receiver handle is accepted but not dereferenced: every entry reaches a
// raise before it would need writer state, and the generated subjects always
// pass a handle produced by XmlWriter.Create.  A 0 handle is therefore
// indistinguishable from a live one here, which is faithful — the BCL checks
// async capability on the instance before it inspects any writer state either.
#include <chaos/native_types.h>
#include <cstring>
#include <string>

#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/xml_writer_async_stubs.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

namespace {

/// The single failure mode of every well-formed call on a non-async writer.
[[noreturn]] void RaiseNotAsyncCapable()
{
    RaiseManagedException(
        "System.InvalidOperationException",
        "An asynchronous operation cannot be started on a writer that is not "
        "configured for asynchronous use.  Set XmlWriterSettings.Async to true "
        "and use the XmlWriter.Create overload that accepts settings.");
}

[[noreturn]] void RaiseArgumentNull(const char* param_name)
{
    RaiseManagedException("System.ArgumentNullException",
                          ("Value cannot be null. (Parameter '" +
                           std::string(param_name) + "')").c_str());
}

[[noreturn]] void RaiseArgument(const char* message)
{
    RaiseManagedException("System.ArgumentException", message);
}

/// Resolve a managed String argument to a view.  Mirrors ManagedStringView in
/// xml_writer_stubs.cpp so the null / empty decisions below match the sync path.
bool StringView(CHAOS_IL2CPP_INTPTR str, const char*& out, size_t& out_len)
{
    out = nullptr;
    out_len = 0;
    if (str == 0) return false;
    if (chaos_is_string_id(str)) {
        const auto view = string_table::Resolve(chaos_extract_string_id(str));
        if (view.utf8_data == nullptr) return false;
        out = view.utf8_data;
        out_len = view.byte_count;
        return true;
    }
    const auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    out = stub_string_data(reinterpret_cast<const void*>(str));
    out_len = static_cast<size_t>(hdr->byte_count);
    return true;
}

/// True when the argument names a non-null, non-empty string.
bool HasText(CHAOS_IL2CPP_INTPTR str)
{
    const char* data = nullptr;
    size_t len = 0;
    if (!StringView(str, data, len)) return false;
    return len > 0;
}

constexpr size_t kSurrogateMin = 0xD800;
constexpr size_t kSurrogateMax = 0xDFFF;

/// Mirror of the sync path's surrogate validation: the high half must be a
/// high surrogate and the low half a low surrogate, in that order.
void ValidateSurrogatePair(CHAOS_IL2CPP_INTPTR low, CHAOS_IL2CPP_INTPTR high)
{
    const auto lo = static_cast<size_t>(low) & 0xFFFFu;
    const auto hi = static_cast<size_t>(high) & 0xFFFFu;
    if (lo < kSurrogateMin || lo > kSurrogateMax || hi < kSurrogateMin || hi > kSurrogateMax) {
        RaiseArgument("Invalid surrogate pair.");
    }
    if (lo >= 0xDC00 || hi < 0xDC00) {
        RaiseArgument("The low surrogate must be in the range 0xDC00-0xDFFF "
                      "and the high surrogate in 0xD800-0xDBFF.");
    }
}

/// Mirror of the sync path's character validation for WriteCharEntity.
void ValidateCharEntity(CHAOS_IL2CPP_INTPTR ch)
{
    const auto value = static_cast<size_t>(ch) & 0xFFFFu;
    if (value >= kSurrogateMin && value <= kSurrogateMax) {
        RaiseArgument("The character is a surrogate and cannot be written as a "
                      "character entity.");
    }
}

/// Buffer/index/count validation shared by the four array-taking entries.
/// The managed array reference is only inspected for identity: a null buffer
/// is rejected, and a negative index/count is rejected, exactly as the sync
/// path does before it would touch any element.
void ValidateArrayRange(CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INTPTR index,
                        CHAOS_IL2CPP_INTPTR count, const char* buffer_param)
{
    if (buffer == 0) RaiseArgumentNull(buffer_param);
    if (index < 0) {
        RaiseManagedException("System.ArgumentOutOfRangeException",
                              "Index was out of range. (Parameter 'index')");
    }
    if (count < 0) {
        RaiseManagedException("System.ArgumentOutOfRangeException",
                              "Count was out of range. (Parameter 'count')");
    }
}

}  // namespace

extern "C" {

// ── 0-arg ────────────────────────────────────────────────────────────────
// No argument validation precedes the capability check: with a fresh writer
// every one of these was measured to raise InvalidOperationException.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStartDocumentAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteEndDocumentAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteEndElementAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteFullEndElementAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterFlushAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterDisposeAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 1-arg: bool ─────────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStartDocumentBoolAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR standalone) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)standalone;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 1-arg: string ───────────────────────────────────────────────────────
// WriteString(null) and WriteRaw(null) are legal (measured OK on .NET 8), so
// they reach the capability check only when text is actually present.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStringAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (!HasText(text)) return 0;  // null / empty writes nothing, and does not throw
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteRawAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR data) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (!HasText(data)) return 0;
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// The remaining string entries validate null before the capability check:
// measured ArgumentNullException on a non-async writer.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteWhitespaceAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ws) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (ws == 0) RaiseArgumentNull("ws");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCommentAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (text == 0) RaiseArgumentNull("text");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCDataAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (text == 0) RaiseArgumentNull("text");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNameAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (name == 0) RaiseArgumentNull("name");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNmTokenAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (name == 0) RaiseArgumentNull("name");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteEntityRefAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (name == 0) RaiseArgumentNull("name");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 1-arg: char ─────────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCharEntityAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ch) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ValidateCharEntity(ch);
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 2-arg: char, char ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteSurrogateCharEntityAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR low,
    CHAOS_IL2CPP_INTPTR high) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ValidateSurrogatePair(low, high);
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── array + offset + count ──────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCharsAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ValidateArrayRange(buffer, index, count, "buffer");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteRawBufferAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ValidateArrayRange(buffer, index, count, "buffer");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteBase64Async(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ValidateArrayRange(buffer, index, count, "buffer");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteBinHexAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ValidateArrayRange(buffer, index, count, "buffer");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 2-arg: string, string ───────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteQualifiedNameAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (local_name == 0) RaiseArgumentNull("localName");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteProcessingInstructionAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (name == 0) RaiseArgumentNull("name");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 3-arg: prefix, localName, ns ────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStartElementAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)prefix;
    (void)ns;
    if (local_name == 0) RaiseArgumentNull("localName");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── 4-arg ───────────────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteDocTypeAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR pubid, CHAOS_IL2CPP_INTPTR sysid,
    CHAOS_IL2CPP_INTPTR subset) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)pubid;
    (void)sysid;
    (void)subset;
    if (name == 0) RaiseArgumentNull("name");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteAttributeStringAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)prefix;
    (void)ns;
    (void)value;
    if (local_name == 0) RaiseArgumentNull("localName");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteElementStringAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)prefix;
    (void)ns;
    (void)value;
    if (local_name == 0) RaiseArgumentNull("localName");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

// ── node / attributes ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNodeAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader, CHAOS_IL2CPP_INTPTR defattr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)defattr;
    if (reader == 0) RaiseArgumentNull("reader");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNodeNavigatorAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR navigator, CHAOS_IL2CPP_INTPTR defattr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)defattr;
    if (navigator == 0) RaiseArgumentNull("navigator");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteAttributesAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader, CHAOS_IL2CPP_INTPTR defattr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    (void)defattr;
    if (reader == 0) RaiseArgumentNull("reader");
    RaiseNotAsyncCapable();
    return 0;  // unreachable: RaiseNotAsyncCapable is [[noreturn]]
}

}  // extern "C"

}  // namespace chaos::il2cpp::runtime_core
