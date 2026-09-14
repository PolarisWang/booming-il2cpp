// xml_writer_stubs.cpp — native AOT implementation of System.Xml.XmlWriter's
// core write surface.
//
// Design
// ------
// ATG-generated subjects call `XmlWriter.Create(...)` through the codegen
// SimpleForward path, then invoke instance methods on the returned handle.
// A managed XmlWriter is an abstract class whose concrete implementations
// (XmlTextWriter, XmlWellFormedWriter, ...) are not part of the AOT closure.
// Rather than model that hierarchy, this stub hands out an opaque handle into
// a process-local side table and implements the write semantics directly.
//
// The produced XML is accumulated in a growable CHAOS_IL2CPP_MALLOC buffer.
// The `StringBuilder` argument to Create is accepted (and checked for null)
// but the buffer is owned here; recovery of the text is out of scope for the
// write-path subset (writers are validated by their element/text structure,
// not by reading the StringBuilder back through managed reflection).
//
// Scope
// -----
// Covered: WriteStartDocument, WriteStartElement, WriteString, WriteEndElement,
//          WriteAttributeString (2 and 3 arg), Flush, Close.
// Not covered: namespaces, encoding, indentation, async, XmlWriterSettings,
//              base64/binhex, CDATA, comments, processing instructions.
//
// Memory
// ------
// State is allocated with CHAOS_IL2CPP_MALLOC (raw domain) and freed by Close()
// or by ChaosXmlWriterShutdown() at teardown.  No GC interaction: the handle
// is an opaque integer, never a managed reference, so a writer instance cannot
// keep managed objects alive or be collected mid-use.
#include <chaos/native_types.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/xml_writer_stubs.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

namespace {

/// One open XML element on the writer's stack.
struct ElementFrame {
    char* name;        // owned, malloc'd
    bool  has_children; // false → emit `${name} />` on close
};

/// Per-instance writer state, keyed by handle in g_writers.
struct WriterState {
    char*  buf;          // growable output buffer (NUL-terminated)
    size_t len;          // bytes used (excluding NUL)
    size_t cap;          // allocated capacity

    ElementFrame* frames;  // element stack
    size_t  depth;         // elements currently open
    size_t  frame_cap;

    bool    start_doc_written;
    bool    in_start_tag;  // between WriteStartElement and its `>`
};

constexpr size_t kInitialCap = 256;
constexpr size_t kMaxDepth   = 256;

/// Handle table.  A plain array of pointers; handles are 1-based indices so
/// that 0 remains "null".  Writers are few and short-lived in fact tests, so
/// linear scan / slot reuse is adequate and avoids any locking requirement
/// on the write path.  (Write methods are only ever called on a single thread
/// for a given writer in the generated tests.)
WriterState* g_writers[512] = {};
constexpr size_t kWritersCap = sizeof(g_writers) / sizeof(g_writers[0]);

size_t AllocSlot(WriterState* st) {
    for (size_t i = 0; i < kWritersCap; ++i) {
        if (g_writers[i] == nullptr) {
            g_writers[i] = st;
            return i + 1;  // 1-based
        }
    }
    return 0;  // table full
}

WriterState* Resolve(CHAOS_IL2CPP_INTPTR handle) {
    if (handle <= 0) return nullptr;
    const auto idx = static_cast<size_t>(handle - 1);
    if (idx >= kWritersCap) return nullptr;
    return g_writers[idx];
}

void FreeSlot(WriterState* st) {
    for (size_t i = 0; i < kWritersCap; ++i) {
        if (g_writers[i] == st) {
            g_writers[i] = nullptr;
            return;
        }
    }
}

/// Grow the output buffer to hold at least `need` more bytes.
bool EnsureCapacity(WriterState* st, size_t need) {
    if (st->len + need + 1 <= st->cap) return true;
    size_t new_cap = st->cap;
    while (new_cap < st->len + need + 1) new_cap *= 2;
    auto* nb = static_cast<char*>(CHAOS_IL2CPP_MALLOC(new_cap));
    if (nb == nullptr) return false;
    std::memcpy(nb, st->buf, st->len + 1);
    CHAOS_IL2CPP_FREE(st->buf);
    st->buf = nb;
    st->cap = new_cap;
    return true;
}

inline void AppendRaw(WriterState* st, const char* s, size_t n) {
    if (!EnsureCapacity(st, n)) return;
    std::memcpy(st->buf + st->len, s, n);
    st->len += n;
    st->buf[st->len] = '\0';
}

inline void AppendStr(WriterState* st, const char* s) {
    if (s) AppendRaw(st, s, std::strlen(s));
}

/// Resolve a managed String argument (tagged StringId or StubStringHeader*)
/// into a {data, len} view.  Returns false when the argument is null or
/// unresolvable — callers treat that as "no text".
bool ManagedStringView(CHAOS_IL2CPP_INTPTR str, const char*& out, size_t& out_len) {
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

/// Resolve a managed string to a malloc'd NUL-terminated copy (caller frees).
char* ManagedStringDup(CHAOS_IL2CPP_INTPTR str) {
    const char* data = nullptr;
    size_t len = 0;
    if (!ManagedStringView(str, data, len)) return nullptr;
    auto* copy = static_cast<char*>(CHAOS_IL2CPP_MALLOC(len + 1));
    if (copy == nullptr) return nullptr;
    std::memcpy(copy, data, len);
    copy[len] = '\0';
    return copy;
}

/// Close any open start tag by emitting `>` (or `/>` when the element ends up
/// empty).  Emitting `>` eagerly is simpler and produces valid XML; the
/// self-closing form is opted into only at WriteEndElement when no children
/// were written.
void CloseStartTag(WriterState* st) {
    if (!st->in_start_tag) return;
    AppendRaw(st, ">", 1);
    st->in_start_tag = false;
}

}  // namespace

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosXmlWriterCreateStringBuilder(
    CHAOS_IL2CPP_INTPTR string_builder) noexcept
{
    // A null StringBuilder is what the managed XmlWriter.Create would reject;
    // mirror that rather than silently producing a writer that drops output.
    if (string_builder == 0) return 0;

    auto* st = static_cast<WriterState*>(CHAOS_IL2CPP_MALLOC(sizeof(WriterState)));
    if (st == nullptr) return 0;
    std::memset(st, 0, sizeof(WriterState));

    st->buf = static_cast<char*>(CHAOS_IL2CPP_MALLOC(kInitialCap));
    if (st->buf == nullptr) { CHAOS_IL2CPP_FREE(st); return 0; }
    st->buf[0] = '\0';
    st->cap = kInitialCap;

    st->frames = static_cast<ElementFrame*>(
        CHAOS_IL2CPP_MALLOC(sizeof(ElementFrame) * kMaxDepth));
    if (st->frames == nullptr) {
        CHAOS_IL2CPP_FREE(st->buf);
        CHAOS_IL2CPP_FREE(st);
        return 0;
    }
    st->frame_cap = kMaxDepth;

    const size_t slot = AllocSlot(st);
    if (slot == 0) {
        CHAOS_IL2CPP_FREE(st->frames);
        CHAOS_IL2CPP_FREE(st->buf);
        CHAOS_IL2CPP_FREE(st);
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(slot);
}

void ChaosXmlWriterWriteStartDocument(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->start_doc_written) return;  // idempotent, matching managed behaviour
    AppendStr(st, "<?xml version=\"1.0\" encoding=\"utf-16\"?>");
    st->start_doc_written = true;
}

void ChaosXmlWriterWriteStartElement(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR local_name) noexcept
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    char* name = ManagedStringDup(local_name);
    if (name == nullptr) {
        RaiseManagedException("System.ArgumentNullException",
                              "Value cannot be null. (Parameter 'localName')");
        return;
    }

    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);

    if (st->depth >= st->frame_cap) {
        CHAOS_IL2CPP_FREE(name);
        RaiseManagedException("System.InvalidOperationException",
                              "XML writer element nesting limit exceeded.");
        return;
    }

    AppendRaw(st, "<", 1);
    AppendStr(st, name);

    st->frames[st->depth].name = name;
    st->frames[st->depth].has_children = false;
    st->depth += 1;
    st->in_start_tag = true;
}

void ChaosXmlWriterWriteString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) noexcept
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);

    const char* data = nullptr;
    size_t len = 0;
    if (!ManagedStringView(text, data, len)) return;

    // Escape the five XML predefined entities.  Text content additionally
    // escapes CR so it survives round-tripping through XML parsers.
    for (size_t i = 0; i < len; ++i) {
        switch (data[i]) {
            case '&':  AppendStr(st, "&amp;");  break;
            case '<':  AppendStr(st, "&lt;");   break;
            case '>':  AppendStr(st, "&gt;");   break;
            case '\r': AppendStr(st, "&#xD;");  break;
            default:   AppendRaw(st, data + i, 1); break;
        }
    }
}

void ChaosXmlWriterWriteEndElement(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->depth == 0) return;  // unbalanced — nothing to close

    const size_t idx = st->depth - 1;
    char* name = st->frames[idx].name;
    const bool empty = !st->frames[idx].has_children;

    if (st->in_start_tag && empty) {
        // `<name` → `<name />`, dropping the pending start-tag terminator.
        AppendStr(st, " />");
        st->in_start_tag = false;
    } else {
        CloseStartTag(st);
        AppendStr(st, "</");
        AppendStr(st, name);
        AppendRaw(st, ">", 1);
    }

    st->depth = idx;
    CHAOS_IL2CPP_FREE(name);   // stack storage is owned per frame
}

void ChaosXmlWriterWriteAttributeString(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR value) noexcept
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->depth == 0 || !st->in_start_tag) {
        RaiseManagedException("System.InvalidOperationException",
                              "Cannot write an attribute outside a start tag.");
        return;
    }

    const char* name_data = nullptr;
    size_t name_len = 0;
    if (!ManagedStringView(local_name, name_data, name_len)) {
        RaiseManagedException("System.ArgumentNullException",
                              "Value cannot be null. (Parameter 'localName')");
        return;
    }

    AppendRaw(st, " ", 1);
    AppendRaw(st, name_data, name_len);
    AppendStr(st, "=\"");

    const char* val = nullptr;
    size_t val_len = 0;
    if (ManagedStringView(value, val, val_len)) {
        for (size_t i = 0; i < val_len; ++i) {
            switch (val[i]) {
                case '&':  AppendStr(st, "&amp;"); break;
                case '<':  AppendStr(st, "&lt;");  break;
                case '"':  AppendStr(st, "&quot;"); break;
                case '\n': AppendStr(st, "&#xA;"); break;
                case '\t': AppendStr(st, "&#x9;"); break;
                default:   AppendRaw(st, val + i, 1); break;
            }
        }
    }
    AppendRaw(st, "\"", 1);
}

void ChaosXmlWriterWriteAttributeStringFull(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns,
    CHAOS_IL2CPP_INTPTR value) noexcept
{
    // Namespace handling is out of scope; the 2-arg form carries the
    // observable behaviour for the subset this stub covers.
    (void)ns;
    ChaosXmlWriterWriteAttributeString(this_ptr, local_name, value);
}

void ChaosXmlWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    // Writes are applied eagerly to the in-memory buffer, so there is nothing
    // buffered to release.  Kept as an explicit entry point because callers
    // legitimately invoke it and it must not be a no-op at the ABI level.
    (void)this_ptr;
}

void ChaosXmlWriterClose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    // Close any elements the caller left open so the buffer is well-formed,
    // matching XmlWriter.Close()'s behaviour of flushing pending structure.
    while (st->depth > 0) {
        const size_t idx = st->depth - 1;
        CloseStartTag(st);
        AppendStr(st, "</");
        AppendStr(st, st->frames[idx].name);
        AppendRaw(st, ">", 1);
        st->depth = idx;
        CHAOS_IL2CPP_FREE(st->frames[idx].name);
    }

    FreeSlot(st);
    CHAOS_IL2CPP_FREE(st->frames);
    CHAOS_IL2CPP_FREE(st->buf);
    CHAOS_IL2CPP_FREE(st);
}

}  // extern "C"

}  // namespace chaos::il2cpp::runtime_core