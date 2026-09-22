// xml_reader_stubs.h — native AOT stub declarations for System.Xml.XmlTextReader
// read-surface methods.  Routes through a simple tokenizer that handles the
// minimal XML subset exercised by ATG-generated fact subjects.
#pragma once

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

extern "C" {

/// XmlTextReader ctor(TextReader input) → opaque handle, or 0 on OOM.
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderCreate(
    CHAOS_IL2CPP_INTPTR input) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.Read() → bool (advance to next node)
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderRead(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_NodeType() → XmlNodeType
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderNodeType(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_Name() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderName(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_LocalName() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderLocalName(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_NamespaceURI() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderNamespaceURI(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_Prefix() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderPrefix(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_Value() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderValue(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_Depth() → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderDepth(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_IsEmptyElement() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderIsEmptyElement(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_HasAttributes() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderHasAttributes(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.get_AttributeCount() → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderAttributeCount(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.MoveToFirstAttribute() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToFirstAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.MoveToNextAttribute() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToNextAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.MoveToElement() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToElement(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.GetAttribute(string name) → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ReadString() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderReadString(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.Close() → void
void ChaosXmlTextReaderClose(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.HasLineInfo() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderHasLineInfo(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.LookupNamespace(string prefix) → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderLookupNamespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ResolveEntity() → void
void ChaosXmlTextReaderResolveEntity(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.MoveToAttribute(string) → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToAttributeStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.MoveToAttribute(string name, string ns) → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToAttributeStrNs(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.GetAttribute(int) → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeIndex(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 index) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.GetAttribute(string, string) → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeStrNs(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ReadContentAsBase64(byte[], int, int) → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadContentAsBase64(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ReadContentAsBinHex(byte[], int, int) → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadContentAsBinHex(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ReadElementContentAsBase64(byte[], int, int) → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadElementContentAsBase64(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ReadElementContentAsBinHex(byte[], int, int) → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderReadElementContentAsBinHex(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR buffer,
    CHAOS_IL2CPP_INT32 index,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.Skip() → void
void ChaosXmlTextReaderSkip(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

/// XmlTextReader.ResetState() → void
void ChaosXmlTextReaderResetState(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT;

}  // extern "C"