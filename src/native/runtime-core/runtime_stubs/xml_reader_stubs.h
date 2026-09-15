// xml_reader_stubs.h — native AOT stub declarations for System.Xml.XmlTextReader
// read-surface methods.  Routes through a simple tokenizer that handles the
// minimal XML subset exercised by ATG-generated fact subjects.
#pragma once

#include <chaos/native_types.h>

extern "C" {

/// XmlTextReader ctor(TextReader input) → opaque handle, or 0 on OOM.
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderCreate(
    CHAOS_IL2CPP_INTPTR input) noexcept;

/// XmlTextReader.Read() → bool (advance to next node)
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderRead(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_NodeType() → XmlNodeType
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderNodeType(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_Name() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderName(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_LocalName() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderLocalName(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_NamespaceURI() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderNamespaceURI(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_Prefix() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderPrefix(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_Value() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderValue(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_Depth() → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderDepth(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_IsEmptyElement() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderIsEmptyElement(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_HasAttributes() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderHasAttributes(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.get_AttributeCount() → int
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderAttributeCount(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.MoveToFirstAttribute() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToFirstAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.MoveToNextAttribute() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToNextAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.MoveToElement() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderMoveToElement(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.GetAttribute(string name) → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderGetAttributeStr(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;

/// XmlTextReader.ReadString() → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderReadString(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.Close() → void
void ChaosXmlTextReaderClose(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.HasLineInfo() → bool
CHAOS_IL2CPP_INT32 ChaosXmlTextReaderHasLineInfo(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.LookupNamespace(string prefix) → string
CHAOS_IL2CPP_INTPTR ChaosXmlTextReaderLookupNamespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix) noexcept;

/// XmlTextReader.ResolveEntity() → void
void ChaosXmlTextReaderResolveEntity(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.Skip() → void
void ChaosXmlTextReaderSkip(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

/// XmlTextReader.ResetState() → void
void ChaosXmlTextReaderResetState(
    CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

}  // extern "C"