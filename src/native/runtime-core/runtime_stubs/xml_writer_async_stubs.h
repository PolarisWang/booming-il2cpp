// xml_writer_async_stubs.h — native AOT declarations for System.Xml.XmlWriter's
// *Async write surface.
//
// Why a separate translation unit
// ------------------------------
// These entries live apart from xml_writer_stubs.{h,cpp} so that the async
// contract stays legible next to the reason it exists.  They also share the
// writer handle table with the sync stubs (see xml_writer_stubs_internal.h),
// so both families resolve the same per-instance WriterState.
//
// Contract
// --------
// ATG's fixture constructs `XmlWriter.Create(new StringBuilder())`, whose
// `Settings.Async` is **false**.  For a writer that is not async-capable the
// BCL's Write*Async methods throw InvalidOperationException **after** the
// argument validation they share with the sync path has passed.  Measured
// against .NET 8 (see the matrix in the .cpp), the ordering is:
//
//   1. invalid argument        -> ArgumentNullException / ArgumentException
//   2. valid argument, non-async writer -> InvalidOperationException
//
// So these stubs deliberately do NOT forward to the sync entries: the sync
// path succeeds (or is idempotent) where the async path must raise.  Each
// *Async entry therefore (a) mirrors the sync argument validation, then
// (b) raises InvalidOperationException.
//
// The Task the managed signature returns is represented by the INTPTR carrier
// the shape registers (same convention as ChaosAsyncTaskDelay in async_stubs).
// These stubs never return normally — every path raises — so the carrier value
// is never observed by generated code.
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

extern "C" {

// ── 0-arg ────────────────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStartDocumentAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteEndDocumentAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteEndElementAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteFullEndElementAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterFlushAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterDisposeAsync(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

// ── 1-arg: bool ─────────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStartDocumentBoolAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR standalone) CHAOS_STUB_NOEXCEPT;

// ── 1-arg: string ───────────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStringAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteWhitespaceAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ws) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCommentAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCDataAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteRawAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR data) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNameAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNmTokenAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteEntityRefAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

// ── 1-arg: char (carried in an int32 slot) ──────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCharEntityAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ch) CHAOS_STUB_NOEXCEPT;

// ── 2-arg: char, char ───────────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteSurrogateCharEntityAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR low,
    CHAOS_IL2CPP_INTPTR high) CHAOS_STUB_NOEXCEPT;

// ── array + offset + count ──────────────────────────────────────────────
// `buffer` is the managed array reference; the stub only needs its identity
// for the null/range checks that precede the async-capability raise.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteCharsAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteRawBufferAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteBase64Async(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteBinHexAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INTPTR index, CHAOS_IL2CPP_INTPTR count) CHAOS_STUB_NOEXCEPT;

// ── 2-arg: string, string ───────────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteQualifiedNameAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteProcessingInstructionAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;

// ── 3-arg: prefix, localName, ns ────────────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteStartElementAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

// ── 4-arg: prefix, localName, ns, value ─────────────────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteDocTypeAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR pubid, CHAOS_IL2CPP_INTPTR sysid,
    CHAOS_IL2CPP_INTPTR subset) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteAttributeStringAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteElementStringAsync(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;

// ── node / attributes: reader or navigator + bool ───────────────────────
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNodeAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader, CHAOS_IL2CPP_INTPTR defattr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteNodeNavigatorAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR navigator, CHAOS_IL2CPP_INTPTR defattr) CHAOS_STUB_NOEXCEPT;
CHAOS_IL2CPP_INTPTR ChaosXmlWriterWriteAttributesAsync(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader, CHAOS_IL2CPP_INTPTR defattr) CHAOS_STUB_NOEXCEPT;

}  // extern "C"
