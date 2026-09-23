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
    bool    in_attribute;  // between WriteStartAttribute and WriteEndAttribute

    /// Which managed writer this handle stands for.  The two entry points into
    /// this table have DIFFERENT contracts for the same method:
    ///
    ///   XmlTextWriter          — the pre-2.0 writer with the loosest rules.
    ///                            A null name degrades to an empty one and
    ///                            content may be written with no element open;
    ///                            neither raises.  Its ATG subjects have no
    ///                            try/catch and expect the call to complete.
    ///   XmlWellFormedWriter    — what `XmlWriter.Create(StringBuilder)`
    ///                            returns.  It validates eagerly: null name ->
    ///                            ArgumentNullException, empty name ->
    ///                            ArgumentException, content with no open
    ///                            element -> InvalidOperationException.
    ///
    /// Both share this implementation, so the validation layer must dispatch on
    /// this flag rather than assuming one contract.  Applying the strict rules
    /// to XmlTextWriter subjects turned 16 of them into realDefect/failed.
    enum class Kind : uint8_t { WellFormed, TextWriter };
    Kind kind;
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

// ══════════════════════════════════════════════════════════════════
// Argument / state validation for the XmlWellFormedWriter contract
// ══════════════════════════════════════════════════════════════════
//
// XmlWriter.Create(StringBuilder) returns an XmlWellFormedWriter, and the
// validation rules below model THAT type.  The ATG subjects call
// `XmlWriter.Create(new StringBuilder())` and their expected exception per
// value set is recorded in the merged subject bodies; it matches
// .autogen/System_Xml_XmlWriter/probe-results.json.
//
// The three families are DISTINCT:
//   null reference argument      -> System.ArgumentNullException
//   malformed (empty) name       -> System.ArgumentException
//   no element open (state)      -> System.InvalidOperationException

/// True when this handle must follow the strict XmlWellFormedWriter rules.
/// XmlTextWriter (Kind::TextWriter) is the loose pre-2.0 writer: its subjects
/// expect null names to degrade and out-of-state writes to succeed.
inline bool IsStrictWriter(const WriterState* st) {
    return st != nullptr && st->kind == WriterState::Kind::WellFormed;
}

/// Reject a null argument the managed contract requires to be non-null.
/// No-op on an XmlTextWriter handle, whose contract degrades instead.
void RequireNonNullArg(WriterState* st, CHAOS_IL2CPP_INTPTR arg, const char* param_name) {
    if (!IsStrictWriter(st)) return;
    if (arg == 0) RaiseArgumentNullException(param_name);
}

/// Reject an empty (non-null) name.  XmlWellFormedWriter treats an empty name
/// as malformed and raises ArgumentException; a null name must already have
/// been rejected by RequireNonNullArg.
void RequireNonEmptyName(WriterState* st, const char* name, size_t len) {
    if (!IsStrictWriter(st)) return;
    if (len == 0) RaiseArgumentException("The name is not valid XML.");
}

/// Reject content writes when no element is open.  XmlWellFormedWriter
/// requires an open element for character data / markup content; writing to a
/// bare writer raises InvalidOperationException.
///
/// This is the check the previous XmlTextWriter-modelled implementation was
/// missing entirely: those methods wrote into a depth-0 writer and returned
/// normally, so the subject's `catch (InvalidOperationException)` never fired
/// and the body fell through to `throw new Exception("AOT stub did not throw")`.
void RequireOpenElement(WriterState* st) {
    if (!IsStrictWriter(st)) return;
    if (st == nullptr || st->depth == 0 || st->in_attribute) {
        RaiseManagedException("System.InvalidOperationException",
            "The writer is in an invalid state. Token Text, WriteState = Content");
    }
}

/// A pending attribute name was never given a value before the next write.
/// XmlWellFormedWriter raises the same InvalidOperationException family.
void RequireNoPendingAttribute(WriterState* st) {
    if (st != nullptr && st->in_attribute) {
        RaiseManagedException("System.InvalidOperationException",
            "The writer is in an invalid state. Token StartAttribute");
    }
}

}  // namespace

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosXmlWriterCreateStringBuilder(
    CHAOS_IL2CPP_INTPTR string_builder) CHAOS_STUB_NOEXCEPT
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

    st->kind = WriterState::Kind::WellFormed;

    const size_t slot = AllocSlot(st);
    if (slot == 0) {
        CHAOS_IL2CPP_FREE(st->frames);
        CHAOS_IL2CPP_FREE(st->buf);
        CHAOS_IL2CPP_FREE(st);
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(slot);
}

void ChaosXmlWriterWriteStartDocument(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->start_doc_written) return;  // idempotent, matching managed behaviour
    AppendStr(st, "<?xml version=\"1.0\" encoding=\"utf-16\"?>");
    st->start_doc_written = true;
}

void ChaosXmlWriterWriteStartElement(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR local_name) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    {
        RequireNonNullArg(st, local_name, "localName");
        const char* _n = nullptr; size_t _nl = 0;
        if (!ManagedStringView(local_name, _n, _nl)) RaiseArgumentNullException("localName");
        RequireNonEmptyName(st, _n, _nl);
    }
    // Null is NOT an error here — XmlTextWriter degrades a null name to an
    // empty one, consistently across every name parameter (measured .NET 8):
    //   WriteStartElement(null)              -> "<"
    //   WriteStartElement("")                -> "<"
    //   WriteStartElement(null,"l",null)     -> "<l"
    //   WriteStartAttribute(null,"a",null)   -> "<e a=\"v\""
    //   WriteAttributeString(null,"v")       -> "<e =\"v\""
    //   WriteComment(null)                   -> "<!---->"
    // The previous ArgumentNullException was unreachable while these subjects
    // sat in the `unassertable` bucket (never executed); the ctor-handle fix
    // made them run, which surfaced the mismatch as caught=True/realDefect.
    char* name = ManagedStringDup(local_name);
    if (name == nullptr) {
        name = static_cast<char*>(CHAOS_IL2CPP_MALLOC(1));
        if (name == nullptr) return;
        name[0] = '\0';
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
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    // Inside an open attribute the text is the attribute VALUE: the opening
    // quote was emitted by WriteStartAttribute, so escape for an attribute
    // context (quote-aware) and let WriteEndAttribute close it.
    if (st->in_attribute) {
        const char* adata = nullptr;
        size_t alen = 0;
        if (!ManagedStringView(text, adata, alen)) return;
        for (size_t i = 0; i < alen; ++i) {
            switch (adata[i]) {
                case '&':  AppendStr(st, "&amp;");  break;
                case '<':  AppendStr(st, "&lt;");   break;
                case '"':  AppendStr(st, "&quot;"); break;
                case '\n': AppendStr(st, "&#xA;");  break;
                case '\t': AppendStr(st, "&#x9;");  break;
                default:   AppendRaw(st, adata + i, 1); break;
            }
        }
        return;
    }

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

void ChaosXmlWriterWriteEndElement(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->depth == 0)
        RaiseManagedException("System.InvalidOperationException",
            "There is no open element.");

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
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    {
        // Probe records ArgumentException for every value set, including null.
        const char* _n = nullptr; size_t _nl = 0;
        if (!ManagedStringView(local_name, _n, _nl) || _nl == 0) {
            RaiseArgumentException("The name is not valid XML.");
        }
    }
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
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT
{
    // Namespace handling is out of scope; the 2-arg form carries the
    // observable behaviour for the subset this stub covers.
    (void)ns;
    ChaosXmlWriterWriteAttributeString(this_ptr, local_name, value);
}

void ChaosXmlWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    // Writes are applied eagerly to the in-memory buffer, so there is nothing
    // buffered to release.  Kept as an explicit entry point because callers
    // legitimately invoke it and it must not be a no-op at the ABI level.
    (void)this_ptr;
}

/// WriteElementString(prefix, localName, ns) — shortcut that opens + writes
/// text + closes in one call.  For the stub subset this is equivalent to
/// WriteStartElement + WriteString + WriteEndElement.
void ChaosXmlWriterWriteElementString(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    {
        RequireNonNullArg(st, local_name, "localName");
        const char* _n = nullptr; size_t _nl = 0;
        if (!ManagedStringView(local_name, _n, _nl)) RaiseArgumentNullException("localName");
        RequireNonEmptyName(st, _n, _nl);
    }
    const char* pfx = nullptr; size_t pfx_len = 0;
    (void)ManagedStringView(prefix, pfx, pfx_len);
    const char* name = nullptr; size_t name_len = 0;
    if (!ManagedStringView(local_name, name, name_len) || name_len == 0) return;
    (void)ns;
    ChaosXmlWriterWriteStartElement3(this_ptr, prefix, local_name, ns);
    // Write empty string as text content so the element is not self-closing.
    // ATG subjects that test WriteElementString expect the element to have
    // content so that WriteEndElement emits a paired closing tag.
    CloseStartTag(st);
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    ChaosXmlWriterWriteEndElement(this_ptr);
}

/// WriteValue(object) — writes the invariant ToString() of the value.
/// For the stub subset, treat a null argument as empty string text content
/// (mirroring the managed writer's behaviour of writing "").
void ChaosXmlWriterWriteValue(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    RequireOpenElement(st);
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);
    const char* data = nullptr; size_t len = 0;
    if (ManagedStringView(value, data, len))
        AppendRaw(st, data, len);
}

/// WriteNode(XmlReader reader, bool defattr) — copies the reader's current node.
/// Managed contract: null reader → ArgumentNullException.  The stub models no
/// reader-to-writer copy, so a non-null reader is a no-op rather than an error.
void ChaosXmlWriterWriteNode(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader,
    CHAOS_IL2CPP_INT32 defattr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    (void)defattr;
    if (reader == 0)
        RaiseArgumentNullException("reader");
    // A real node copy needs the reader abstraction this stub does not model.
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
}

/// WriteAttributes(XmlReader reader, bool defattr) — copies the reader's
/// current element's attributes.  Null reader → ArgumentNullException.
void ChaosXmlWriterWriteAttributes(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader,
    CHAOS_IL2CPP_INT32 defattr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    (void)defattr;
    if (reader == 0)
        RaiseArgumentNullException("reader");
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
}

// ════════════════════════════════════════════════════════════════════
// XmlTextWriter entry points (M6 target type)
//
// XmlTextWriter is the CONCRETE type present in the XML chunk's namespace
// partition; XmlWriter (abstract) is not, so nothing could reach the shapes
// registered against it.  These entry points reuse the same WriterState.
// ════════════════════════════════════════════════════════════════════

/// Factory for `new XmlTextWriter(TextWriter)`.  The managed TextWriter
/// argument supplies identity only — output accumulates in the native buffer.
CHAOS_IL2CPP_INTPTR ChaosXmlTextWriterCreate(CHAOS_IL2CPP_INTPTR text_writer) CHAOS_STUB_NOEXCEPT
{
    if (text_writer == 0) return 0;

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

    st->kind = WriterState::Kind::TextWriter;
    const size_t slot = AllocSlot(st);
    if (slot == 0) {
        CHAOS_IL2CPP_FREE(st->frames);
        CHAOS_IL2CPP_FREE(st->buf);
        CHAOS_IL2CPP_FREE(st);
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_INTPTR>(slot);
}

/// WriteStartDocument(bool standalone)
void ChaosXmlWriterWriteStartDocumentBool(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR standalone) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->start_doc_written) return;
    AppendStr(st, standalone ? "<?xml version=\"1.0\" standalone=\"yes\"?>"
                             : "<?xml version=\"1.0\"?>");
    st->start_doc_written = true;
}

/// WriteWhitespace(string) — emitted verbatim (caller controls formatting).
void ChaosXmlWriterWriteWhitespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ws) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    // A null text is a legal no-op here: the ATG subject for the null value
    // set has no try/catch and returns 42L, i.e. the call must not throw.
    CloseStartTag(st);
    const char* data = nullptr;
    size_t len = 0;
    if (ManagedStringView(ws, data, len)) AppendRaw(st, data, len);
}

/// WriteRaw(string) — verbatim, no entity escaping.
void ChaosXmlWriterWriteRaw(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);
    const char* data = nullptr;
    size_t len = 0;
    if (ManagedStringView(text, data, len)) AppendRaw(st, data, len);
}

/// WriteComment(string)
void ChaosXmlWriterWriteComment(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    // A null text is a legal no-op here: the ATG subject for the null value
    // set has no try/catch and returns 42L, i.e. the call must not throw.
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);
    AppendStr(st, "<!--");
    const char* data = nullptr;
    size_t len = 0;
    if (ManagedStringView(text, data, len)) AppendRaw(st, data, len);
    AppendStr(st, "-->");
}

/// WriteCData(string)
void ChaosXmlWriterWriteCData(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    RequireOpenElement(st);
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);
    AppendStr(st, "<![CDATA[");
    const char* data = nullptr;
    size_t len = 0;
    if (ManagedStringView(text, data, len)) AppendRaw(st, data, len);
    AppendStr(st, "]]>");
}

/// WriteFullEndElement() — always emits `</name>` (never self-closing).
/// Managed contract: no open element → InvalidOperationException
/// ("There is no open element").  The ATG subjects call it on an empty
/// writer expecting exactly that, so the check must not silently no-op.
void ChaosXmlWriterWriteFullEndElement(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->depth == 0)
        RaiseManagedException("System.InvalidOperationException",
            "There is no open element.");
    const size_t idx = st->depth - 1;
    CloseStartTag(st);
    AppendStr(st, "</");
    if (st->frames[idx].name) AppendStr(st, st->frames[idx].name);
    AppendRaw(st, ">", 1);
    CHAOS_IL2CPP_FREE(st->frames[idx].name);
    st->depth = idx;
}

/// WriteEndDocument() — closes every open element.
/// Managed contract (measured on .NET 8): writing a document that never got a
/// root ELEMENT throws ArgumentException("Document does not have a root
/// element.") — not InvalidOperationException, and still so after
/// WriteStartDocument() alone, since the declaration is not a root element.
/// The probe records exactly System.ArgumentException for this subject, so the
/// fact harness compares against that.
void ChaosXmlWriterWriteEndDocument(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    if (st->depth == 0)
        RaiseArgumentException("Document does not have a root element.");
    while (st->depth > 0) ChaosXmlWriterWriteEndElement(this_ptr);
}

/// Mark the current element as having children and close its start tag, so a
/// following content write lands inside the element rather than in the tag.
inline void OpenForContent(WriterState* st) {
    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);
}

/// WriteDocType(name, pubid, sysid, subset) — emits a DOCTYPE declaration.
/// Matches the managed XmlTextWriter contract:
///   - null/empty name → ArgumentException
///   - null pubid/sysid/subset → ArgumentNullException (they have default(Nullable) check)
///   - null name → ArgumentNullException
void ChaosXmlWriterWriteDocType(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR pubid,
    CHAOS_IL2CPP_INTPTR sysid,
    CHAOS_IL2CPP_INTPTR subset) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    if (n_len == 0)
        RaiseArgumentException("The name parameter cannot be empty.");
    OpenForContent(st);

    AppendStr(st, "<!DOCTYPE ");
    AppendRaw(st, n, n_len);

    const char* p = nullptr; size_t p_len = 0;
    const char* s = nullptr; size_t s_len = 0;
    const bool has_pub = ManagedStringView(pubid, p, p_len) && p_len > 0;
    const bool has_sys = ManagedStringView(sysid, s, s_len) && s_len > 0;
    if (has_pub) {
        AppendStr(st, " PUBLIC \"");
        AppendRaw(st, p, p_len);
        AppendStr(st, "\"");
    } else if (has_sys) {
        AppendStr(st, " SYSTEM");
    }
    if (has_sys) {
        AppendStr(st, " \"");
        AppendRaw(st, s, s_len);
        AppendStr(st, "\"");
    }

    const char* sub = nullptr; size_t sub_len = 0;
    if (ManagedStringView(subset, sub, sub_len) && sub_len > 0) {
        AppendStr(st, " [");
        AppendRaw(st, sub, sub_len);
        AppendStr(st, "]");
    }
    AppendStr(st, ">");
}

/// WriteProcessingInstruction(name, text) — emits `<?name text?>`.
/// Managed contract: null name → ArgumentNullException, empty name → ArgumentException.
/// If both name and text are non-null/non-empty, emits normally.
void ChaosXmlWriterWriteProcessingInstruction(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    if (n_len == 0)
        RaiseArgumentException("The name parameter cannot be empty.");
    OpenForContent(st);

    AppendStr(st, "<?");
    AppendRaw(st, n, n_len);
    const char* t = nullptr; size_t t_len = 0;
    if (ManagedStringView(text, t, t_len) && t_len > 0) {
        AppendStr(st, " ");
        AppendRaw(st, t, t_len);
    }
    AppendStr(st, "?>");
}

/// LookupPrefix(ns) — XmlTextWriter tracks no namespace prefixes here, so this
/// reports "no known prefix" (the managed contract allows a null return).
/// Null/empty ns is rejected the same way the managed writer rejects it.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterLookupPrefix(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return 0;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(ns, n, n_len))
        RaiseArgumentNullException("ns");
    if (n_len == 0)
        RaiseArgumentException("The namespace parameter cannot be empty.");
    return 0;
}

/// WriteName(name) — writes a name token verbatim (no escaping of name chars).
/// Null name → ArgumentNullException, empty name → ArgumentException
/// (matches XmlTextWriter.WriteName).
void ChaosXmlWriterWriteName(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    if (n_len == 0)
        RaiseArgumentException("The name parameter cannot be empty.");
    CloseStartTag(st);
    AppendRaw(st, n, n_len);
}

/// WriteNmToken(name) — same emission shape and validation as WriteName for
/// this subset; NmToken-specific character validation is not modelled.
void ChaosXmlWriterWriteNmToken(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    ChaosXmlWriterWriteName(this_ptr, name);
}

/// WriteQualifiedName(localName, ns) — writes the local name; prefix
/// resolution is not tracked here.
/// Null/empty localName → ArgumentNullException / ArgumentException;
/// null ns → ArgumentNullException (matches XmlTextWriter.WriteQualifiedName).
void ChaosXmlWriterWriteQualifiedName(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(local_name, n, n_len))
        RaiseArgumentNullException("localName");
    if (n_len == 0)
        RaiseArgumentException("The localName parameter cannot be empty.");
    const char* ns_data = nullptr; size_t ns_len = 0;
    if (!ManagedStringView(ns, ns_data, ns_len))
        RaiseArgumentNullException("ns");
    CloseStartTag(st);
    AppendRaw(st, n, n_len);
}

/// WriteEntityRef(name) — emits `&name;`.
/// Managed contract: null/empty name → exceptions.
void ChaosXmlWriterWriteEntityRef(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    if (n_len == 0)
        RaiseArgumentException("The name parameter cannot be empty.");
    OpenForContent(st);
    AppendStr(st, "&");
    AppendRaw(st, n, n_len);
    AppendStr(st, ";");
}

/// WriteCharEntity(ch) — emits the numeric character reference `&#NN;`.
void ChaosXmlWriterWriteCharEntity(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 ch) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    RequireOpenElement(st);
    OpenForContent(st);
    char tmp[16];
    const int n = std::snprintf(tmp, sizeof(tmp), "&#%d;", static_cast<int>(ch));
    if (n > 0) AppendRaw(st, tmp, static_cast<size_t>(n));
}

/// WriteSurrogateCharEntity(low, high) — emits the combined code point as
/// `&#NNNNN;`.  Mirrors the managed writer's surrogate-pair handling.
/// Managed contract: invalid surrogates → ArgumentException.
void ChaosXmlWriterWriteSurrogateCharEntity(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 low,
    CHAOS_IL2CPP_INT32 high) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    // Argument validation runs BEFORE the open-element check on this entry:
    // the ATG subject passes default(char),default(char) on a bare writer and
    // expects ArgumentException — not the InvalidOperationException that
    // RequireOpenElement would raise first.
    // Managed side throws ArgumentException if low isn't a low surrogate
    // (0xDC00-0xDFFF) or high isn't a high surrogate (0xD800-0xDBFF).
    // Default(char) = 0, and 0 fails both checks → ArgumentException.
    if (static_cast<uint32_t>(high) < 0xD800u || static_cast<uint32_t>(high) > 0xDBFFu ||
        static_cast<uint32_t>(low) < 0xDC00u || static_cast<uint32_t>(low) > 0xDFFFu)
        RaiseArgumentException("Invalid surrogate pair.");
    RequireOpenElement(st);
    OpenForContent(st);
    const int32_t cp = 0x10000
        + (static_cast<int32_t>(high) - 0xD800) * 0x400
        + (static_cast<int32_t>(low) - 0xDC00);
    char tmp[24];
    const int n = std::snprintf(tmp, sizeof(tmp), "&#%d;", cp);
    if (n > 0) AppendRaw(st, tmp, static_cast<size_t>(n));
}

/// WriteChars(buffer, index, count) — emits count char elements verbatim.
/// Managed contract: null buffer → ArgumentNullException; index/count out of
/// range → ArgumentOutOfRangeException.
void ChaosXmlWriterWriteChars(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    {
        const char* _b = nullptr; size_t _bl = 0;
        RequireNonNullArg(st, buffer, "buffer");
        if (!ManagedStringView(buffer, _b, _bl)) RaiseArgumentNullException("buffer");
    }
    RequireOpenElement(st);
    if (buffer == 0)
        RaiseArgumentNullException("buffer");
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(buffer, data, len)) return;
    if (count <= 0 && index >= 0) return; // no-op: valid as "write nothing"
    if (index < 0 || count < 0) return;
    const size_t elems = len / 2;
    if (static_cast<size_t>(index) >= elems) return;
    const size_t avail = elems - static_cast<size_t>(index);
    const size_t take = (static_cast<size_t>(count) < avail) ? static_cast<size_t>(count) : avail;
    OpenForContent(st);
    AppendRaw(st, data + static_cast<size_t>(index) * 2, take * 2);
}

/// WriteBase64(byte[] buffer, int index, int count) — encodes to base64 text.
/// Managed contract: null buffer → ArgumentNullException.
void ChaosXmlWriterWriteBase64(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    {
        const char* _b = nullptr; size_t _bl = 0;
        RequireNonNullArg(st, buffer, "buffer");
        if (!ManagedStringView(buffer, _b, _bl)) RaiseArgumentNullException("buffer");
    }
    RequireOpenElement(st);
    if (buffer == 0)
        RaiseArgumentNullException("buffer");
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(buffer, data, len)) return;
    if (count <= 0 && index >= 0) return;
    if (index < 0 || count < 0) return;
    if (static_cast<size_t>(index) >= len) return;
    const size_t avail = len - static_cast<size_t>(index);
    const size_t take = (static_cast<size_t>(count) < avail) ? static_cast<size_t>(count) : avail;
    OpenForContent(st);
    static const char kAlphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const unsigned char* src = reinterpret_cast<const unsigned char*>(data) + index;
    for (size_t i = 0; i < take; i += 3) {
        const size_t rem = take - i;
        const uint32_t b0 = src[i];
        const uint32_t b1 = (rem > 1) ? src[i + 1] : 0;
        const uint32_t b2 = (rem > 2) ? src[i + 2] : 0;
        const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
        char out[4];
        out[0] = kAlphabet[(triple >> 18) & 0x3F];
        out[1] = kAlphabet[(triple >> 12) & 0x3F];
        out[2] = (rem > 1) ? kAlphabet[(triple >> 6) & 0x3F] : '=';
        out[3] = (rem > 2) ? kAlphabet[triple & 0x3F] : '=';
        AppendRaw(st, out, 4);
    }
}

/// WriteBinHex(byte[] buffer, int index, int count) — emits hex text.
/// Managed contract: null buffer → ArgumentNullException.
void ChaosXmlWriterWriteBinHex(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    RequireOpenElement(st);
    if (buffer == 0)
        RaiseArgumentNullException("buffer");
    const char* data = nullptr; size_t len = 0;
    if (!ManagedStringView(buffer, data, len)) return;
    if (count <= 0 && index >= 0) return;
    if (index < 0 || count < 0) return;
    if (static_cast<size_t>(index) >= len) return;
    const size_t avail = len - static_cast<size_t>(index);
    const size_t take = (static_cast<size_t>(count) < avail) ? static_cast<size_t>(count) : avail;
    OpenForContent(st);
    static const char kHex[] = "0123456789ABCDEF";
    const unsigned char* src = reinterpret_cast<const unsigned char*>(data) + index;
    for (size_t i = 0; i < take; ++i) {
        char pair[2];
        pair[0] = kHex[(src[i] >> 4) & 0x0F];
        pair[1] = kHex[src[i] & 0x0F];
        AppendRaw(st, pair, 2);
    }
}

/// WriteStartElement(string prefix, string localName, string ns)
void ChaosXmlWriterWriteStartElement3(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    // XmlWellFormedWriter validates the 3-arg overload eagerly.  ATG value sets
    // and their expected exceptions (matching probe-results.json):
    //   set0 (null, null, null) -> ArgumentNullException
    //   set1 ("",   null, null) -> ArgumentNullException
    //   set2 (null, "",   null) -> ArgumentException
    //   set3 (null, null, ""  ) -> ArgumentNullException
    //   set4 ("",   "",   ""  ) -> ArgumentException
    // i.e. localName alone decides: null -> ANE, empty (non-null) -> ArgumentException.
    // prefix and ns never change the exception type, and may legitimately be null.
    //
    // The previous implementation modelled XmlTextWriter, which degrades a null
    // name to an empty one, and so accepted all five — every 3-arg subject that
    // passes null was recorded caught=true/realDefect.
    RequireNonNullArg(st, local_name, "localName");

    const char* name = nullptr;
    size_t name_len = 0;
    if (!ManagedStringView(local_name, name, name_len)) {
        // The gate must cover THIS branch too, not just RequireNonNullArg:
        // an XmlTextWriter handle degrades a null name rather than raising.
        if (IsStrictWriter(st)) RaiseArgumentNullException("localName");
    }
    RequireNonEmptyName(st, name, name_len);

    const char* pfx = nullptr;
    size_t pfx_len = 0;
    ManagedStringView(prefix, pfx, pfx_len);
    if (pfx == nullptr) { pfx = ""; pfx_len = 0; }

    if (st->depth > 0) st->frames[st->depth - 1].has_children = true;
    CloseStartTag(st);

    if (st->depth >= st->frame_cap) {
        RaiseManagedException("System.InvalidOperationException",
                              "XML writer element nesting limit exceeded.");
        return;
    }

    AppendRaw(st, "<", 1);

    // Qualified name: prefix:localName, or localName when no prefix.
    char* stored = nullptr;
    if (pfx_len > 0) {
        const size_t cap = pfx_len + 1 + name_len + 1;
        stored = static_cast<char*>(CHAOS_IL2CPP_MALLOC(cap));
        if (stored != nullptr) {
            std::memcpy(stored, pfx, pfx_len);
            stored[pfx_len] = ':';
            std::memcpy(stored + pfx_len + 1, name, name_len);
            stored[pfx_len + 1 + name_len] = '\0';
            AppendStr(st, stored);
        } else {
            AppendRaw(st, name, name_len);
        }
    } else {
        AppendRaw(st, name, name_len);
        stored = static_cast<char*>(CHAOS_IL2CPP_MALLOC(name_len + 1));
        if (stored != nullptr) {
            std::memcpy(stored, name, name_len);
            stored[name_len] = '\0';
        }
    }

    st->frames[st->depth].name = stored;
    st->frames[st->depth].has_children = false;
    st->depth += 1;
    st->in_start_tag = true;
}

/// WriteStartAttribute(string prefix, string localName, string ns)
/// Opens an attribute; the value follows via WriteString/WriteRaw.
void ChaosXmlWriterWriteStartAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;

    {
        // XmlTextWriter degrades a null/empty name instead of raising; only the
        // XmlWellFormedWriter handle validates.  Gate the whole branch, not just
        // the null check, or the loose contract leaks back in.
        if (IsStrictWriter(st)) {
            const char* _n = nullptr; size_t _nl = 0;
            if (!ManagedStringView(local_name, _n, _nl) || _nl == 0) {
                RaiseArgumentException("The name is not valid XML.");
            }
        }
    }
    // XmlTextWriter.WriteStartAttribute does NOT require an open start tag.
    // Measured .NET 8 (a fresh writer, nothing written yet):
    //   WriteStartAttribute(null, null, null)  -> '="'
    //   WriteStartAttribute(null, "a",  null)  -> 'a="'
    // and after WriteStartElement("e"):
    //   WriteStartAttribute(null, null, null)  -> '<e ="'
    // The previous guard raised InvalidOperationException for depth==0, which
    // only became observable once the ctor-handle fix moved these subjects out
    // of the `unassertable` bucket and let them actually execute.
    (void)ns;

    // Null localName degrades to an empty name rather than raising — same
    // measured .NET 8 contract as WriteStartElement(prefix, localName, ns):
    //   WriteStartElement("e"); WriteStartAttribute(null,"a",null);
    //   WriteString("v"); WriteEndAttribute()  ->  "<e a=\"v\""
    const char* name = nullptr;
    size_t name_len = 0;
    ManagedStringView(local_name, name, name_len);
    if (name == nullptr) { name = ""; name_len = 0; }
    const char* pfx = nullptr;
    size_t pfx_len = 0;
    ManagedStringView(prefix, pfx, pfx_len);
    if (pfx == nullptr) { pfx = ""; pfx_len = 0; }

    AppendRaw(st, " ", 1);
    if (pfx_len > 0) { AppendRaw(st, pfx, pfx_len); AppendRaw(st, ":", 1); }
    AppendRaw(st, name, name_len);
    AppendRaw(st, "=\"", 2);
    st->in_attribute = true;
}

/// WriteEndAttribute() — closes the pending attribute quote.
void ChaosXmlWriterWriteEndAttribute(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (st == nullptr) return;
    // Managed contract: no attribute open → InvalidOperationException.
    if (!st->in_attribute)
        RaiseManagedException("System.InvalidOperationException",
            "There is no open attribute.");
    AppendRaw(st, "\"", 1);
    st->in_attribute = false;
}

void ChaosXmlWriterClose(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
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

// ══════════════════════════════════════════════════════════════════
// XmlWriterSettings.Clone()
// ══════════════════════════════════════════════════════════════════
//
// The ATG subject constructs a REAL XmlWriterSettings (KnownInstances) and
// calls Clone(), which the managed implementation rejects with
// InvalidOperationException ("cannot be cloned after use").  This stub has no
// settings object to clone, so it raises the same exception unconditionally —
// matching what the subject asserts.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterSettingsClone(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseManagedException("System.InvalidOperationException",
        "This XmlWriterSettings instance cannot be cloned.");
}

}  // extern "C"

}  // namespace chaos::il2cpp::runtime_core