// xml_writer_stubs.h — native AOT stub declarations for System.Xml.XmlWriter
// core write-surface methods.  Each SimpleForward-registered method routes
// through the handle table to a per-instance ChaosXmlWriterState.
#pragma once

#include <chaos/native_types.h>

extern "C" {

/// XmlWriter.Create(StringBuilder) → opaque handle, or 0 on OOM.
CHAOS_IL2CPP_INTPTR ChaosXmlWriterCreateStringBuilder(
    CHAOS_IL2CPP_INTPTR string_builder) noexcept;

/// XmlWriter.WriteStartDocument() → void
void ChaosXmlWriterWriteStartDocument(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlWriter.WriteStartElement(string localName) → void
void ChaosXmlWriterWriteStartElement(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR local_name) noexcept;

/// XmlWriter.WriteString(string text) → void
void ChaosXmlWriterWriteString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) noexcept;

/// XmlWriter.WriteEndElement() → void
void ChaosXmlWriterWriteEndElement(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlWriter.WriteAttributeString(string localName, string value) → void
void ChaosXmlWriterWriteAttributeString(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR value) noexcept;

/// XmlWriter.WriteAttributeString(string localName, string ns, string value) → void
void ChaosXmlWriterWriteAttributeStringFull(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns,
    CHAOS_IL2CPP_INTPTR value) noexcept;

/// XmlWriter.Flush() → void
void ChaosXmlWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlWriter.Close() → void (frees handle slot)
void ChaosXmlWriterClose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

}  // extern "C"