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

extern "C" {

/// NameTable.Add(string name) → string (interned, reference-stable).
/// Returns a tagged StringId for the interned string, or 0 (null) on OOM/null.
/// Throws NullReferenceException when `this` is null.
CHAOS_IL2CPP_INTPTR ChaosXmlNameTableAddString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;

/// XmlNameTable.Get(string name) → string? (query-only).
/// Returns the interned tagged StringId if present, 0 (null) if absent.
/// Throws NullReferenceException when `this` is null.
CHAOS_IL2CPP_INTPTR ChaosXmlNameTableGetString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;

}  // extern "C"
