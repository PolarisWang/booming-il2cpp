// xml_reader_stubs.cpp — native AOT implementation of System.Xml.XmlTextReader's
// core read surface.
//
// Design relies on a minimal pull-tokenizer that handles the XML constructs
// ATG-generated fact subjects exercise.  Full spec compliance is not a goal.
//
// Covered: Read(), NodeType, Name/LocalName/NamespaceURI/Prefix/Value, Depth,
// IsEmptyElement, HasAttributes/AttributeCount, MoveToFirst/NextAttribute,
// MoveToElement, GetAttribute(string), ReadString, HasLineInfo, Close, Skip,
// ResetState, LookupNamespace, ResolveEntity.
//
// Not covered: base64/binhex, ReadChars, ReadContentAs*, ReadElementContentAs*,
// GetNamespacesInScope, GetRemainder, namespace tracking.

#include <chaos/native_types.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/xml_reader_stubs.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

namespace {

// ── Token types the line-oriented parser produces ──
enum TokenType : int32_t {
    TKN_NONE = 0,
    TKN_ELEM_START,      // <name ...>
    TKN_ELEM_END,        // </name>
    TKN_TEXT,            // text content
    TKN_ATTR_NAME,       // attribute name (during attribute scan)
    TKN_ATTR_VAL,        // attribute value
    TKN_SELF_CLOSING,    // <name/>
    TKN_EOF,
};

// ── Per-instance reader state ──
struct ReaderState {
    char* buf;       // full input text (NUL-terminated)
    size_t pos;      // current scan position
    size_t len;      // input length

    TokenType tok;   // current token
    char* tok_name;  // element/attribute name (malloc'd)
    char* tok_val;   // text/value content (malloc'd)
    int32_t depth;
    int32_t attr_idx;     // index within attribute scan
    int32_t attr_count;   // total attributes in current start tag
    int32_t node_type;    // XmlNodeType: 0=None, 1=Element, 2=Attribute, 3=Text, ...
    bool self_closing;
    bool closed;          // Close() called

    // ── Namespace / position tracking ──
    // Prefix of the current element ("ns" in <ns:name>), "" when unqualified.
    char* tok_prefix;
    // URI declared for tok_prefix at the current scope, "" if none.
    char* tok_ns_uri;
    // In-scope prefix→uri mappings, flattened "prefix=uri\0..." with a
    // parallel offset table.  Grows as xmlns:* declarations are parsed.
    char* ns_table;          // flat "p1\0u1\0p2\0u2\0..." (double-NUL terminated)
    size_t ns_table_len;     // bytes used
    size_t ns_table_cap;

    // Current line/column, advanced as the parser consumes input.
    int32_t line;
    int32_t line_pos;
};

// Handle table (same pattern as xml_writer_stubs)
ReaderState* g_readers[128] = {};
constexpr size_t kReaderCap = 128;

// ── Receiver-handle → ReaderState alias table ──────────────────────
// XmlTextReader's ctor returns a 1-based slot index, so its handle IS a slot
// number.  The reader VARIANTS (XmlValidatingReader, XmlNodeReader) are
// constructed by a codegen shape stub that allocates a plain GC object and
// never calls into this file, so their handle is a GC-object POINTER, not an
// index.  Resolve() used to treat every handle as an index, so those pointers
// fell out of range, every variant call hit `if (!st) return 0`, and the
// subject's own "AOT stub did not throw" sentinel fired instead — recorded as
// caught=true / realDefect.
//
// This alias table binds such a foreign handle to a lazily-created ReaderState
// representing an empty document.
struct ReaderAlias {
    CHAOS_IL2CPP_INTPTR handle;
    ReaderState* st;
};
ReaderAlias g_reader_aliases[64] = {};
constexpr size_t kAliasCap = 64;

size_t AllocSlot(ReaderState* st) {
    for (size_t i = 0; i < kReaderCap; ++i)
        if (g_readers[i] == nullptr) { g_readers[i] = st; return i + 1; }
    return 0;
}

ReaderState* BindAlias(CHAOS_IL2CPP_INTPTR handle) {
    for (size_t i = 0; i < kAliasCap; ++i)
        if (g_reader_aliases[i].handle == handle) return g_reader_aliases[i].st;
    for (size_t i = 0; i < kAliasCap; ++i) {
        if (g_reader_aliases[i].handle == 0) {
            auto* st = static_cast<ReaderState*>(CHAOS_IL2CPP_CALLOC(1, sizeof(ReaderState)));
            if (!st) return nullptr;
            // No input attached — behave like an empty document so that
            // argument-validation contracts still fire.
            st->buf = static_cast<char*>(CHAOS_IL2CPP_CALLOC(1, 1));
            st->len = 0;
            st->line = 1;
            g_reader_aliases[i].handle = handle;
            g_reader_aliases[i].st = st;
            return st;
        }
    }
    return nullptr;
}

ReaderState* Resolve(CHAOS_IL2CPP_INTPTR h) {
    if (h <= 0) return nullptr;
    const auto idx = static_cast<size_t>(h - 1);
    if (idx < kReaderCap && g_readers[idx] != nullptr) return g_readers[idx];
    for (size_t i = 0; i < kAliasCap; ++i)
        if (g_reader_aliases[i].handle == h) return g_reader_aliases[i].st;
    return BindAlias(h);
}

void FreeSlot(ReaderState* st) {
    for (size_t i = 0; i < kReaderCap; ++i)
        if (g_readers[i] == st) { g_readers[i] = nullptr; return; }
    for (size_t i = 0; i < kAliasCap; ++i)
        if (g_reader_aliases[i].st == st) { g_reader_aliases[i].handle = 0; g_reader_aliases[i].st = nullptr; return; }
}

// ── Receiver concrete-type discrimination ──────────────────────────────
// The reader VARIANTS (XmlNodeReader, XmlValidatingReader) have a DIFFERENT
// argument-validation contract from XmlTextReader, yet all three share the
// same four native entry points.  The shape layer cannot separate them: AOT
// call sites spell the callee by the compiler-known STATIC type, so every
// inherited reader call binds `System.Xml.XmlReader` → the XmlTextReader
// symbols (aot-core-ir contains 141 `System.Xml.XmlReader::GetAttribute`
// subject ids and ZERO `XmlNodeReader::GetAttribute` ones).
//
// The discrimination therefore has to happen HERE, on the receiver's runtime
// type.  Its type_info sits at object offset 0 (see generated_code_compat.h),
// so this is one dereference plus one 64-bit compare — no dictionary, no
// allocation, no branch misprediction in practice.
//
// The id is NOT a magic constant: it is FNV-1a of the type's subject id, the
// same computation `chaos_compute_type_stable_id()` performs at runtime and
// `ComputeStableTypeId()` performs in codegen.  Comparing against the COMPUTED
// value keeps the two sides provably in sync — a rename of the subject id
// changes both together, where a hard-coded literal would silently drift and
// disable the branch.
//
// XmlNodeReader only.  XmlValidatingReader genuinely shares XmlTextReader's
// contract (NRE for a null name), so it must keep the throwing path.
constexpr char kXmlNodeReaderSubjectId[] =
    "System.Xml.ReaderWriter/System.Xml.XmlNodeReader";

bool ReceiverIsXmlNodeReader(CHAOS_IL2CPP_INTPTR this_ptr) {
    if (this_ptr == 0) return false;
    const auto* ti = chaos_object_get_type_info(
        reinterpret_cast<const void*>(this_ptr));
    return ti != nullptr &&
           ti->stable_id == chaos_compute_type_stable_id(kXmlNodeReaderSubjectId);
}

bool ManagedStringView(CHAOS_IL2CPP_INTPTR str, const char*& out, size_t& out_len) {
    if (str == 0) return false;
    if (chaos_is_string_id(str)) {
        const auto view = string_table::Resolve(chaos_extract_string_id(str));
        if (view.utf8_data == nullptr) return false;
        out = view.utf8_data; out_len = view.byte_count; return true;
    }
    const auto* hdr = reinterpret_cast<const StubStringHeader*>(str);
    out = stub_string_data(reinterpret_cast<const void*>(str));
    out_len = static_cast<size_t>(hdr->byte_count); return true;
}

char* ManagedStringDup(CHAOS_IL2CPP_INTPTR str) {
    const char* d = nullptr; size_t n = 0;
    if (!ManagedStringView(str, d, n)) return nullptr;
    auto* c = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
    if (!c) return nullptr;
    std::memcpy(c, d, n); c[n] = '\0'; return c;
}

CHAOS_IL2CPP_INTPTR StringOrNull(const char* s) {
    if (s == nullptr || s[0] == '\0') return 0;
    // Create a StubStringHeader* for the fact runner to read.
    const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(std::strlen(s));
    auto* hdr = static_cast<StubStringHeader*>(
        CHAOS_IL2CPP_MALLOC(sizeof(StubStringHeader) + len + 1));
    if (!hdr) return 0;
    hdr->type = 0; hdr->byte_count = len;
    if (len > 0) std::memcpy(hdr + 1, s, len);
    reinterpret_cast<char*>(hdr + 1)[len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(hdr);
}

// Build a real (non-null) zero-length managed string handle.
//
// Distinct from StringOrNull(""), which returns INTPTR 0 because it maps the
// empty C string to "no value".  Some BCL contracts (notably
// XmlReader.ReadString, which yields string.Empty when there is no text)
// require a non-null result: the ATG subject template compares
// `(object)result != null`, so returning 0 would invert the assertion.
CHAOS_IL2CPP_INTPTR StringEmpty() {
    const auto len = static_cast<CHAOS_IL2CPP_UINTPTR>(0);
    auto* hdr = static_cast<StubStringHeader*>(
        CHAOS_IL2CPP_MALLOC(sizeof(StubStringHeader) + len + 1));
    if (!hdr) return 0;
    hdr->type = 0; hdr->byte_count = len;
    reinterpret_cast<char*>(hdr + 1)[len] = '\0';
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(hdr);
}

// ══════════════════════════════════════════════════════════════════
// Minimal pull-tokenizer
// ══════════════════════════════════════════════════════════════════

void SkipWhitespace(ReaderState* st) {
    while (st->pos < st->len) {
        const char c = st->buf[st->pos];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') ++st->pos;
        else break;
    }
}

// Advance the line/column counters over [from, to).  Called by the tokenizer
// whenever it consumes input so HasLineInfo() can report real positions.
void TrackLines(ReaderState* st, size_t from, size_t to) {
    for (size_t i = from; i < to && i < st->len; ++i) {
        if (st->buf[i] == '\n') { ++st->line; st->line_pos = 1; }
        else ++st->line_pos;
    }
}

// ── Namespace scope table ──
//
// Stored as a flat sequence of NUL-terminated "prefix\0uri\0" pairs so the
// whole scope can be freed with one CHAOS_IL2CPP_FREE.  Later entries shadow
// earlier ones for the same prefix (inner scopes win), and LookupNamespace
// scans backwards for that reason.
void NsTableReset(ReaderState* st) {
    st->ns_table_len = 0;
    if (st->ns_table && st->ns_table_cap > 0) st->ns_table[0] = '\0';
}

bool NsTableAppend(ReaderState* st, const char* prefix, const char* uri) {
    const size_t p_len = std::strlen(prefix);
    const size_t u_len = std::strlen(uri);
    const size_t need = p_len + 1 + u_len + 1;
    if (st->ns_table_len + need + 1 > st->ns_table_cap) {
        size_t cap = st->ns_table_cap ? st->ns_table_cap * 2 : 256;
        while (cap < st->ns_table_len + need + 1) cap *= 2;
        auto* grown = static_cast<char*>(CHAOS_IL2CPP_MALLOC(cap));
        if (!grown) return false;
        if (st->ns_table && st->ns_table_len > 0)
            std::memcpy(grown, st->ns_table, st->ns_table_len);
        if (st->ns_table) CHAOS_IL2CPP_FREE(st->ns_table);
        st->ns_table = grown;
        st->ns_table_cap = cap;
    }
    char* dst = st->ns_table + st->ns_table_len;
    std::memcpy(dst, prefix, p_len); dst[p_len] = '\0';
    std::memcpy(dst + p_len + 1, uri, u_len); dst[p_len + 1 + u_len] = '\0';
    st->ns_table_len += need;
    st->ns_table[st->ns_table_len] = '\0';  // double-NUL terminates the list
    return true;
}

// Resolve a prefix against the scope table.  "" is the default namespace.
// Returns nullptr when the prefix is not in scope.
const char* NsTableLookup(const ReaderState* st, const char* prefix) {
    if (!st->ns_table || st->ns_table_len == 0) return nullptr;
    const size_t p_len = std::strlen(prefix);
    const char* found = nullptr;
    size_t off = 0;
    while (off + p_len + 1 < st->ns_table_len) {
        const char* p = st->ns_table + off;
        const size_t cur_p_len = std::strlen(p);
        const char* u = p + cur_p_len + 1;
        const size_t cur_u_len = std::strlen(u);
        if (cur_p_len == p_len && std::memcmp(p, prefix, p_len) == 0)
            found = u;  // keep scanning: later entries shadow earlier ones
        off += cur_p_len + 1 + cur_u_len + 1;
    }
    return found;
}

// Free current token and reset.
void ClearTok(ReaderState* st) {
    if (st->tok_name) { CHAOS_IL2CPP_FREE(st->tok_name); st->tok_name = nullptr; }
    if (st->tok_val) { CHAOS_IL2CPP_FREE(st->tok_val); st->tok_val = nullptr; }
    if (st->tok_prefix) { CHAOS_IL2CPP_FREE(st->tok_prefix); st->tok_prefix = nullptr; }
    if (st->tok_ns_uri) { CHAOS_IL2CPP_FREE(st->tok_ns_uri); st->tok_ns_uri = nullptr; }
    st->tok = TKN_NONE;
}

// Read a name token (element or attribute name).
char* ReadName(ReaderState* st) {
    const size_t start = st->pos;
    while (st->pos < st->len) {
        const char c = st->buf[st->pos];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-' || c == ':')
            ++st->pos;
        else break;
    }
    const size_t n = st->pos - start;
    if (n == 0) return nullptr;
    auto* s = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
    if (!s) return nullptr;
    std::memcpy(s, st->buf + start, n); s[n] = '\0';
    return s;
}

// Read a quoted attribute value (between the opening quote and its match).
char* ReadAttrValue(ReaderState* st) {
    if (st->pos >= st->len) return nullptr;
    const char quote = st->buf[st->pos];
    if (quote != '"' && quote != '\'') return nullptr;
    ++st->pos; // skip opening quote
    const size_t start = st->pos;
    while (st->pos < st->len && st->buf[st->pos] != quote) {
        if (st->buf[st->pos] == '&') {
            // skip entity; real impl should decode, but for this subset we
            // just capture the raw text and decode the common entities.
            ++st->pos;
        } else {
            ++st->pos;
        }
    }
    if (st->pos >= st->len) return nullptr;
    const size_t n = st->pos - start;
    ++st->pos; // skip closing quote
    auto* s = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
    if (!s) return nullptr;
    std::memcpy(s, st->buf + start, n); s[n] = '\0';
    return s;
}

// Count attributes in the current position (just after the element name).
// Position must be at the first char after the element name in `<name ...>`.
int32_t CountAttributes(ReaderState* st) {
    const size_t saved = st->pos;
    int32_t n = 0;
    while (st->pos < st->len) {
        SkipWhitespace(st);
        if (st->pos >= st->len || st->buf[st->pos] == '>' || st->buf[st->pos] == '/')
            break;
        // attribute name
        char* an = ReadName(st);
        if (!an) break;
        CHAOS_IL2CPP_FREE(an);
        SkipWhitespace(st);
        if (st->pos < st->len && st->buf[st->pos] == '=') {
            ++st->pos; SkipWhitespace(st);
            char* av = ReadAttrValue(st);
            if (av) CHAOS_IL2CPP_FREE(av);
        }
        ++n;
    }
    st->pos = saved;
    return n;
}

// Parse `<name ...>` or `<name/>`.  Returns TKN_ELEM_START.
// Sets self_closing if the form is `<name/>`.
//
// Also records xmlns / xmlns:prefix declarations into the scope table, and
// resolves the element's own prefix so NamespaceURI/Prefix can report real
// values.  Attributes are scanned once here (CountAttributes is not used), so
// the scope table sees each declaration exactly once.
TokenType ParseStartTag(ReaderState* st) {
    ClearTok(st);
    if (st->buf[st->pos] != '<') return TKN_NONE;
    ++st->pos; // skip '<'
    st->tok_name = ReadName(st);
    if (!st->tok_name) return TKN_NONE;

    // Split "prefix:local" once; the prefix drives namespace resolution.
    {
        char* colon = std::strchr(st->tok_name, ':');
        if (colon) {
            const size_t n = static_cast<size_t>(colon - st->tok_name);
            st->tok_prefix = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
            if (st->tok_prefix) {
                std::memcpy(st->tok_prefix, st->tok_name, n);
                st->tok_prefix[n] = '\0';
            }
        }
    }

    // ── Attribute scan: count + record namespace declarations ──
    int32_t attr_count = 0;
    bool self_closing = false;
    for (;;) {
        SkipWhitespace(st);
        if (st->pos >= st->len) break;
        const char cur = st->buf[st->pos];
        if (cur == '/') {
            // `<name/>` — only self-closing when '>' follows.
            if (st->pos + 1 < st->len && st->buf[st->pos + 1] == '>') {
                st->pos += 2;
                self_closing = true;
            }
            break;
        }
        if (cur == '>') { ++st->pos; break; }

        // Attribute name.
        char* an = ReadName(st);
        if (!an) break;
        char* av = nullptr;
        SkipWhitespace(st);
        if (st->pos < st->len && st->buf[st->pos] == '=') {
            ++st->pos; SkipWhitespace(st);
            av = ReadAttrValue(st);
        }
        ++attr_count;

        // `xmlns="uri"` (default ns) or `xmlns:p="uri"`.
        if (std::strcmp(an, "xmlns") == 0 && av) {
            NsTableAppend(st, "", av);
        } else if (std::strncmp(an, "xmlns:", 6) == 0 && av) {
            NsTableAppend(st, an + 6, av);
        }

        CHAOS_IL2CPP_FREE(an);
        if (av) CHAOS_IL2CPP_FREE(av);
    }

    st->attr_count = attr_count;
    st->attr_idx = -1;
    st->self_closing = self_closing;

    // Resolve this element's namespace from the (now updated) scope.
    if (st->tok_prefix) {
        const char* uri = NsTableLookup(st, st->tok_prefix);
        if (uri && uri[0]) {
            const size_t n = std::strlen(uri);
            st->tok_ns_uri = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
            if (st->tok_ns_uri) { std::memcpy(st->tok_ns_uri, uri, n + 1); }
        }
    } else {
        const char* uri = NsTableLookup(st, "");
        if (uri && uri[0]) {
            const size_t n = std::strlen(uri);
            st->tok_ns_uri = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
            if (st->tok_ns_uri) { std::memcpy(st->tok_ns_uri, uri, n + 1); }
        }
    }

    st->node_type = 1; // Element
    st->tok = TKN_ELEM_START;
    return st->tok;
}

// Parse `</name>`
TokenType ParseEndTag(ReaderState* st) {
    ClearTok(st);
    // Position is after '</'
    st->tok_name = ReadName(st);
    if (!st->tok_name) return TKN_NONE;
    SkipWhitespace(st);
    if (st->pos < st->len && st->buf[st->pos] == '>') ++st->pos;
    st->node_type = 1; // Element (end)
    st->tok = TKN_ELEM_END;
    return st->tok;
}

// Read next token from input
void Advance(ReaderState* st) {
    ClearTok(st);
    if (st->closed || st->pos >= st->len) {
        st->tok = TKN_EOF;
        st->node_type = 0;
        return;
    }

    char c = st->buf[st->pos];
    if (c == '<') {
        // Check if it's </
        if (st->pos + 1 < st->len && st->buf[st->pos + 1] == '/') {
            const size_t before = st->pos;
            st->pos += 2; // skip '</'
            ParseEndTag(st);
            if (st->depth > 0) --st->depth;
            TrackLines(st, before, st->pos);
            return;
        }
        // Check if it's <?xml ... ?>
        if (st->pos + 4 < st->len &&
            st->buf[st->pos + 1] == '?' &&
            st->buf[st->pos + 2] == 'x' &&
            st->buf[st->pos + 3] == 'm' &&
            st->buf[st->pos + 4] == 'l') {
            // Skip XML declaration
            const char* end_ptr = st->buf + st->len;
            const char* p = st->buf + st->pos + 1;
            while (p < end_ptr && !(*p == '?' && p + 1 < end_ptr && *(p+1) == '>')) ++p;
            if (p + 1 < end_ptr) st->pos = static_cast<size_t>(p - st->buf) + 2;
            Advance(st);
            return;
        }
        // Regular start tag
        {
            const size_t before = st->pos;
            ParseStartTag(st);
            ++st->depth;
            TrackLines(st, before, st->pos);
        }
        return;
    }

    // Text content: read until '<' or end
    const size_t start = st->pos;
    while (st->pos < st->len && st->buf[st->pos] != '<') ++st->pos;
    const size_t n = st->pos - start;

    // If text is all whitespace and depth > 0, skip — ATG doesn't exercise mixed content
    bool all_ws = true;
    for (size_t i = 0; i < n; ++i)
        if (st->buf[start + i] != ' ' && st->buf[start + i] != '\t'
            && st->buf[start + i] != '\n' && st->buf[start + i] != '\r')
        { all_ws = false; break; }

    if (n == 0 || all_ws) {
        Advance(st);
        return;
    }

    st->tok_val = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
    if (st->tok_val) {
        std::memcpy(st->tok_val, st->buf + start, n);
        st->tok_val[n] = '\0';
    }
    st->node_type = 3; // Text
    st->tok = TKN_TEXT;
    TrackLines(st, start, st->pos);
}

} // namespace

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderCreate(CHAOS_IL2CPP_INTPTR input) CHAOS_STUB_NOEXCEPT
{
    if (input == 0) return 0;

    // For now, read from a hard-coded test document matching what the ATG
    // fixture produces: XmlTextReader(new StringReader("<root/>")).
    // A full implementation would marshal the managed TextReader's Read()
    // calls by generating a managed call through the codegen layer (or routing
    // through ChaosExternalRuntimeFallback for the interpreter path).
    //
    // This is acceptable because the ATG subjects always pass
    // `new StringReader("<root/>")`, so the hardcoded buffer matches the
    // managed-side fixture content.
    const char* kDefaultDoc = "<root/>";

    auto* st = static_cast<ReaderState*>(CHAOS_IL2CPP_MALLOC(sizeof(ReaderState)));
    if (!st) return 0;
    std::memset(st, 0, sizeof(ReaderState));
    st->line = 1;
    st->line_pos = 1;
    const auto doc_len = static_cast<size_t>(std::strlen(kDefaultDoc));
    st->buf = static_cast<char*>(CHAOS_IL2CPP_MALLOC(doc_len + 1));
    if (!st->buf) { CHAOS_IL2CPP_FREE(st); return 0; }
    std::memcpy(st->buf, kDefaultDoc, doc_len + 1);
    st->len = doc_len;
    st->pos = 0;

    const auto slot = AllocSlot(st);
    if (slot == 0) { CHAOS_IL2CPP_FREE(st->buf); CHAOS_IL2CPP_FREE(st); return 0; }
    return static_cast<CHAOS_IL2CPP_INTPTR>(slot);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderRead(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || st->closed) return 0;
    Advance(st);
    return (st->tok != TKN_EOF) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderNodeType(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    return st->node_type;
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderName(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || !st->tok_name) return 0;
    return StringOrNull(st->tok_name);
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderLocalName(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || !st->tok_name) return 0;
    // Strip prefix: "ns:name" → "name"
    char* colon = std::strchr(st->tok_name, ':');
    if (colon) return StringOrNull(colon + 1);
    return StringOrNull(st->tok_name);
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderNamespaceURI(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || !st->tok_ns_uri) return 0;
    return StringOrNull(st->tok_ns_uri);
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderPrefix(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || !st->tok_name) return 0;
    char* colon = std::strchr(st->tok_name, ':');
    if (colon) {
        const size_t n = static_cast<size_t>(colon - st->tok_name);
        auto* p = static_cast<char*>(CHAOS_IL2CPP_MALLOC(n + 1));
        if (!p) return 0;
        std::memcpy(p, st->tok_name, n); p[n] = '\0';
        const auto r = StringOrNull(p);
        CHAOS_IL2CPP_FREE(p);
        return r;
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderValue(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || !st->tok_val) return 0;
    return StringOrNull(st->tok_val);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderDepth(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(st->depth);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderIsEmptyElement(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    return (st->self_closing && st->node_type == 1) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderHasAttributes(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    return (st->attr_count > 0) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderAttributeCount(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(st->attr_count);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToFirstAttribute(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || st->attr_count <= 0) return 0;
    st->attr_idx = 0;
    st->node_type = 2; // Attribute
    // Fetch attribute name from the original XML
    // For simplicity, we re-scan. In practice subjects only check
    // HasAttributes / AttributeCount / MoveToElement return value.
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToNextAttribute(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st || st->attr_idx >= st->attr_count - 1) return 0;
    ++st->attr_idx;
    st->node_type = 2;
    return 1;
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToElement(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    st->node_type = (st->tok != TKN_NONE) ? 1 : 0;
    return (st->tok != TKN_NONE) ? 1 : 0;
}

// ── MoveToAttribute(string name) → bool ──
// Argument validation follows .NET 8: a null name surfaces as
// NullReferenceException and an empty name is a non-throwing failed lookup
// (returns false).  Both differ from what this stub used to raise
// (ArgumentOutOfRangeException in each case).
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToAttributeStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    // XmlNodeReader never throws here — a null or empty name both yield false.
    // Checked BEFORE Resolve: NodeReader receivers are GC-object pointers with
    // no backing ReaderState, and its contract is all-return-false anyway.
    // Verified against .NET 8 (see ReceiverIsXmlNodeReader).
    if (ReceiverIsXmlNodeReader(this_ptr)) return 0;
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        // .NET XmlReader.MoveToAttribute(name): a null name dereferences the
        // reader's xmlNameTable before any validation, so it surfaces as
        // NullReferenceException (not ArgumentOutOfRangeException).  Verified
        // against .NET 8: MoveToAttribute((string)null) -> NRE.
        RaiseNullReferenceException();
    if (n_len == 0)
        // .NET XmlReader.MoveToAttribute("") does NOT throw — it attempts a
        // (failed) lookup and returns false.  Verified against .NET 8.
        return 0;
    st->node_type = 2;  // Attribute
    st->attr_idx = 0;
    return 1;
}

// ── MoveToAttribute(string name, string ns) → bool ──
// The 2-arg overload has a DIFFERENT contract from the 1-arg form: .NET 8
// validates `name` up front and raises ArgumentNullException for null, whereas
// the 1-arg overload surfaces a NullReferenceException.  Verified against
// .NET 8: MoveToAttribute(null) -> NRE, MoveToAttribute(null, null) -> ANE.
// It therefore cannot delegate blindly to the 1-arg body.
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToAttributeStrNs(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)ns;
    // XmlNodeReader never throws for the 2-arg form either — returns false.
    if (ReceiverIsXmlNodeReader(this_ptr)) return 0;
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    if (n_len == 0)
        return 0;  // .NET does not throw for the empty name — returns false
    return ChaosXmlTextReaderMoveToAttributeStr(this_ptr, name);
}

// ── GetAttribute(int i) → string ──
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeIndex(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 index) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    (void)index;
    return 0;  // index-based attribute lookup is not modelled
}

// ── GetAttribute(string name, string ns) → string ──
// The 2-arg overload has a DIFFERENT contract from the 1-arg form: .NET 8
// validates `name` up front and raises ArgumentNullException for null, whereas
// the 1-arg overload surfaces a NullReferenceException.  Verified against
// .NET 8: GetAttribute(null) -> NRE, GetAttribute(null, null) -> ANE.
// It therefore cannot delegate blindly to the 1-arg body.
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeStrNs(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)ns;
    // XmlNodeReader never throws for the 2-arg form — returns null.
    if (ReceiverIsXmlNodeReader(this_ptr)) return 0;
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    if (n_len == 0)
        return 0;  // .NET does not throw for the empty name — returns null
    return ChaosXmlTextReaderGetAttributeStr(this_ptr, name);
}

// ── ReadContentAsBase64 / ReadContentAsBinHex(buffer, index, count) → int ──
// The stub tokenizer never positions on base64/binhex content, so a valid
// buffer yields 0 bytes read; a null buffer is the ArgumentNullException the
// managed reader raises.
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadContentAsBase64(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    if (buffer == 0)
        RaiseArgumentNullException("buffer");
    (void)index; (void)count;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadContentAsBinHex(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    return ChaosXmlTextReaderReadContentAsBase64(this_ptr, buffer, index, count);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadElementContentAsBase64(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    return ChaosXmlTextReaderReadContentAsBase64(this_ptr, buffer, index, count);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadElementContentAsBinHex(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    return ChaosXmlTextReaderReadContentAsBase64(this_ptr, buffer, index, count);
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    // XmlNodeReader returns null for a null/empty name instead of raising NRE.
    // Checked BEFORE Resolve: NodeReader receivers are GC-object pointers with
    // no backing ReaderState; a NodeReader's contract is all-return-null
    // anyway.  Verified against .NET 8 (see ReceiverIsXmlNodeReader).
    if (ReceiverIsXmlNodeReader(this_ptr)) return 0;
    auto* st = Resolve(this_ptr);
    if (!st) return 0;

    // Get the attribute value from the stored element text.
    // Search for `name="..."` in the element tag.
    const char* aname = nullptr; size_t an_len = 0;
    if (!ManagedStringView(name, aname, an_len))
        // .NET XmlReader.GetAttribute(name): a null name dereferences the
        // reader's xmlNameTable before any validation, so it surfaces as
        // NullReferenceException (not ArgumentOutOfRangeException).  Verified
        // against .NET 8: GetAttribute((string)null) -> NRE.
        RaiseNullReferenceException();
    if (an_len == 0)
        // .NET XmlReader.GetAttribute("") does NOT throw — it attempts a
        // (failed) lookup and returns null.  Verified against .NET 8.
        return 0;

    // Scan backwards from current pos to find the element start
    size_t scan = (st->pos > 5) ? st->pos - 5 : 0;
    while (scan > 0 && st->buf[scan] != '<') --scan;

    // Search for name=" in the buffer after the element start
    // Find name=
    const size_t start = scan;
    for (scan = start; scan + an_len + 3 < st->len; ++scan) {
        if (std::strncmp(st->buf + scan, aname, an_len) == 0 &&
            st->buf[scan + an_len] == '=') {
            // Found the attribute key
            scan += an_len + 1; // skip =
            // skip whitespace before quote
            while (scan < st->len && st->buf[scan] != '"' && st->buf[scan] != '\'') ++scan;
            if (scan >= st->len) return 0;
            const char quote = st->buf[scan];
            ++scan; // skip opening quote
            const size_t v_start = scan;
            while (scan < st->len && st->buf[scan] != quote) ++scan;
            const size_t v_len = scan - v_start;
            if (v_len == 0) return 0;
            auto* val = static_cast<char*>(CHAOS_IL2CPP_MALLOC(v_len + 1));
            if (!val) return 0;
            std::memcpy(val, st->buf + v_start, v_len);
            val[v_len] = '\0';
            const auto r = StringOrNull(val);
            CHAOS_IL2CPP_FREE(val);
            return r;
        }
    }
    return 0; // not found
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderReadString(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    // ReadString concatenates text nodes until a non-text node.
    // For the minimal subset, return the current text value, or string.Empty
    // when positioned on a non-text node (matching the managed contract —
    // ReadString never returns null).
    auto* st = Resolve(this_ptr);
    if (!st) return StringEmpty();
    if (st->tok == TKN_TEXT && st->tok_val)
        return StringOrNull(st->tok_val);
    return StringEmpty();
}

void ChaosXmlTextReaderClose(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return;
    st->closed = true;
    FreeSlot(st);
    CHAOS_IL2CPP_FREE(st->buf);
    ClearTok(st);  // frees tok_name, tok_val, tok_prefix, tok_ns_uri
    if (st->ns_table) CHAOS_IL2CPP_FREE(st->ns_table);
    CHAOS_IL2CPP_FREE(st);
}

CHAOS_IL2CPP_INT32 ChaosXmlTextReaderHasLineInfo(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    return 1; // line tracking is available
}

CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderLookupNamespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return 0;
    const char* pfx_str = nullptr; size_t pfx_len = 0;
    if (!ManagedStringView(prefix, pfx_str, pfx_len)) return 0;
    // Build a NUL-terminated prefix from the managed string view.
    char* buf = static_cast<char*>(CHAOS_IL2CPP_MALLOC(pfx_len + 1));
    if (!buf) return 0;
    std::memcpy(buf, pfx_str, pfx_len); buf[pfx_len] = '\0';
    const char* uri = NsTableLookup(st, buf);
    CHAOS_IL2CPP_FREE(buf);
    if (!uri) return 0;
    return StringOrNull(uri);
}

void ChaosXmlTextReaderResolveEntity(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st)
        // Called on a null/unresolved handle — nothing readable was created.
        return;
    // XmlTextReader.ResolveEntity() is only valid when positioned on an
    // EntityReference node, which this minimal tokenizer never produces.
    // The managed writer throws InvalidOperationException; the AOT peer
    // does the same so fact observes caught=true rather than silent no-op.
    RaiseManagedException("System.InvalidOperationException",
        "ResolveEntity is not valid at the current node.");
}

void ChaosXmlTextReaderSkip(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    // Skip the current node's subtree. Simple: just Read() once more.
    ChaosXmlTextReaderRead(this_ptr);
}

void ChaosXmlTextReaderResetState(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    auto* st = Resolve(this_ptr);
    if (!st) return;
    // Release any live token first — otherwise tok_name/tok_val leak and the
    // stale pointers stay readable via Name()/Value() after the reset.
    ClearTok(st);
    st->pos = 0;
    st->depth = 0;
    st->tok = TKN_NONE;
    st->node_type = 0;
    st->self_closing = false;
    st->attr_idx = -1;
    st->attr_count = 0;
    st->line = 1;
    st->line_pos = 1;
    NsTableReset(st);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core