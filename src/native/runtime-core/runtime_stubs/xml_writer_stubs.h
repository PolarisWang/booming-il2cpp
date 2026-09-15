// xml_writer_stubs.h — native AOT stub declarations for System.Xml.XmlTextWriter
// write-surface methods.  Each SimpleForward-registered method routes through
// the handle table to a per-instance WriterState (see xml_writer_stubs.cpp).
//
// Target type is XmlTextWriter (concrete), NOT XmlWriter (abstract): the latter
// is not present in the XML chunk's namespace partition, so registering it
// produced shapes no subject could reach.
#pragma once

#include <chaos/native_types.h>

extern "C" {

/// XmlTextWriter ctor → opaque handle, or 0 on OOM.
/// Mirrors `new XmlTextWriter(TextWriter)`; the managed argument provides the
/// sink identity only — output is accumulated in the native side buffer.
CHAOS_IL2CPP_INTPTR ChaosXmlTextWriterCreate(CHAOS_IL2CPP_INTPTR text_writer) noexcept;

/// XmlTextWriter.WriteStartDocument() → void
void ChaosXmlWriterWriteStartDocument(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextWriter.WriteStartDocument(bool standalone) → void
void ChaosXmlWriterWriteStartDocumentBool(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR standalone) noexcept;

/// XmlTextWriter.WriteStartElement(string prefix, string localName, string ns) → void
void ChaosXmlWriterWriteStartElement3(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) noexcept;

/// XmlTextWriter.WriteString(string text) → void
void ChaosXmlWriterWriteString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) noexcept;

/// XmlTextWriter.WriteWhitespace(string ws) → void (emitted verbatim)
void ChaosXmlWriterWriteWhitespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ws) noexcept;

/// XmlTextWriter.WriteEndElement() → void
void ChaosXmlWriterWriteEndElement(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextWriter.WriteFullEndElement() → void
void ChaosXmlWriterWriteFullEndElement(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextWriter.WriteEndDocument() → void
void ChaosXmlWriterWriteEndDocument(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextWriter.WriteStartAttribute(string prefix, string localName, string ns) → void
void ChaosXmlWriterWriteStartAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) noexcept;

/// XmlTextWriter.WriteEndAttribute() → void (closes a pending attribute quote)
void ChaosXmlWriterWriteEndAttribute(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextWriter.WriteRaw(string) → void (verbatim, no escaping)
void ChaosXmlWriterWriteRaw(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) noexcept;

/// XmlTextWriter.WriteComment(string) → void
void ChaosXmlWriterWriteComment(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) noexcept;

/// XmlTextWriter.WriteCData(string) → void
void ChaosXmlWriterWriteCData(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) noexcept;

/// XmlTextWriter.Flush() → void
void ChaosXmlWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextWriter.Close() → void (frees handle slot)
void ChaosXmlWriterClose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

}  // extern "C"