// xml_writer_stubs.h — native AOT stub declarations for System.Xml.XmlTextWriter
// write-surface methods.  Each SimpleForward-registered method routes through
// the handle table to a per-instance WriterState (see xml_writer_stubs.cpp).
//
// Target type is XmlTextWriter (concrete), NOT XmlWriter (abstract): the latter
// is not present in the XML chunk's namespace partition, so registering it
// produced shapes no subject could reach.
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

extern "C" {

/// XmlTextWriter ctor → opaque handle, or 0 on OOM.
/// Mirrors `new XmlTextWriter(TextWriter)`; the managed argument provides the
/// sink identity only — output is accumulated in the native side buffer.
CHAOS_IL2CPP_INTPTR ChaosXmlTextWriterCreate(CHAOS_IL2CPP_INTPTR text_writer) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteStartDocument() → void
void ChaosXmlWriterWriteStartDocument(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteStartDocument(bool standalone) → void
void ChaosXmlWriterWriteStartDocumentBool(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR standalone) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteStartElement(string prefix, string localName, string ns) → void
void ChaosXmlWriterWriteStartElement3(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteString(string text) → void
void ChaosXmlWriterWriteString(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteWhitespace(string ws) → void (emitted verbatim)
void ChaosXmlWriterWriteWhitespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ws) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteEndElement() → void
void ChaosXmlWriterWriteEndElement(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteFullEndElement() → void
void ChaosXmlWriterWriteFullEndElement(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteEndDocument() → void
void ChaosXmlWriterWriteEndDocument(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteStartAttribute(string prefix, string localName, string ns) → void
void ChaosXmlWriterWriteStartAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteEndAttribute() → void (closes a pending attribute quote)
void ChaosXmlWriterWriteEndAttribute(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteRaw(string) → void (verbatim, no escaping)
void ChaosXmlWriterWriteRaw(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteComment(string) → void
void ChaosXmlWriterWriteComment(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteCData(string) → void
void ChaosXmlWriterWriteCData(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteDocType(string name, string pubid, string sysid, string subset) → void
void ChaosXmlWriterWriteDocType(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR pubid,
    CHAOS_IL2CPP_INTPTR sysid,
    CHAOS_IL2CPP_INTPTR subset) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteProcessingInstruction(string name, string text) → void
void ChaosXmlWriterWriteProcessingInstruction(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR text) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteChars(char[] buffer, int index, int count) → void
void ChaosXmlWriterWriteChars(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteName(string) → void
void ChaosXmlWriterWriteName(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteNmToken(string) → void
void ChaosXmlWriterWriteNmToken(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteQualifiedName(string localName, string ns) → void
void ChaosXmlWriterWriteQualifiedName(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteEntityRef(string name) → void
void ChaosXmlWriterWriteEntityRef(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteCharEntity(char ch) → void
void ChaosXmlWriterWriteCharEntity(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 ch) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteSurrogateCharEntity(char low, char high) → void
void ChaosXmlWriterWriteSurrogateCharEntity(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 low,
    CHAOS_IL2CPP_INT32 high) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteBase64(byte[] buffer, int index, int count) → void
void ChaosXmlWriterWriteBase64(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteBinHex(byte[] buffer, int index, int count) → void
void ChaosXmlWriterWriteBinHex(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteAttributeString(string localName, string value) → void
void ChaosXmlWriterWriteAttributeString(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteElementString(string prefix, string localName, string ns) → void
void ChaosXmlWriterWriteElementString(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteValue(object value) → void
void ChaosXmlWriterWriteValue(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.LookupPrefix(string ns) → string (null when unknown)
CHAOS_IL2CPP_INTPTR ChaosXmlWriterLookupPrefix(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteNode(XmlReader reader, bool defattr) → void
void ChaosXmlWriterWriteNode(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader,
    CHAOS_IL2CPP_INT32 defattr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.WriteAttributes(XmlReader reader, bool defattr) → void
void ChaosXmlWriterWriteAttributes(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR reader,
    CHAOS_IL2CPP_INT32 defattr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.Flush() → void
void ChaosXmlWriterFlush(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextWriter.Close() → void (frees handle slot)
void ChaosXmlWriterClose(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlWriterSettings.Clone() → XmlWriterSettings
/// Raises InvalidOperationException (no settings object to clone).
CHAOS_IL2CPP_INTPTR ChaosXmlWriterSettingsClone(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

}  // extern "C"