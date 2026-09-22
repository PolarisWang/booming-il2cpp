// ── XmlNameTable stub declarations ─────────────────────────────
// Implements System.Xml.NameTable / System.Xml.XmlNameTable interned-string
// semantics in native AOT:
//   * Add(string)  — atomic intern; same content returns the same interned
//                    string reference (reference-equality contract).
//   * Get(string)  — query-only lookup; never inserts, returns null if absent.
//
// A managed String in the AOT runtime is a tagged StringId (CHAOS_STRING_ID_TAG
// | id) backed by the permanent string_table, OR a real StubStringHeader*. Both
// forms are handled by these stubs. The interned string is owned by the
// immutable string pool (never GC-collected → permanently rooted, no extra GC
// allocation domain), which satisfies NameTable's "returns the same String
// reference" guarantee for a given content.
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT

extern "C" {

/// NameTable.Add(string name) → string (interned, reference-stable).
/// Returns a tagged StringId for the interned string, or 0 (null) on OOM/null.
/// Throws NullReferenceException when `this` is null.
CHAOS_IL2CPP_INTPTR ChaosXmlNameTableAddString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// XmlNameTable.Get(string name) → string? (query-only).
/// Returns the interned tagged StringId if present, 0 (null) if absent.
/// Throws NullReferenceException when `this` is null.
CHAOS_IL2CPP_INTPTR ChaosXmlNameTableGetString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// NameTable.Add(char[] key, int start, int len) → string (interned).
/// Same reference-equality contract as the string overload; the char[] window
/// is decoded UTF-16 → UTF-8 before interning.  `key` is a managed char[]
/// (StubArrayHeader with 2-byte elements); the window is [start, start+len).
/// Throws NullReferenceException when `this` is null and
/// ArgumentNullException when `key` is null — matching .NET.
CHAOS_IL2CPP_INTPTR ChaosXmlNameTableAddChars(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR key,
    CHAOS_IL2CPP_INT32 start, CHAOS_IL2CPP_INT32 len) CHAOS_STUB_NOEXCEPT;

/// NameTable.Get(char[] key, int start, int len) → string? (query-only).
/// Mirror of AddChars; never inserts.  Returns 0 (null) when absent.
CHAOS_IL2CPP_INTPTR ChaosXmlNameTableGetChars(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR key,
    CHAOS_IL2CPP_INT32 start, CHAOS_IL2CPP_INT32 len) CHAOS_STUB_NOEXCEPT;

}  // extern "C"
