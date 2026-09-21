// xml_document_stubs.h — native AOT declarations for System.Xml DOM types.
//
// These stubs model the argument-validation and bare-object contract of
// XmlDocument / XmlNode / XmlElement / XmlAttribute / XmlCharacterData.
// ATG constructs them through SubjectInstanceFactory.Create<T>() (a
// GetUninitializedObject instance) whose instance methods throw in managed
// code; the native side raises the same exception types so the AOT subject
// observes an equivalent result instead of a silent no-op.
//
// Methods that require a real DOM tree raise NotSupportedException on their
// valid-input paths; their null-argument paths raise ArgumentNullException,
// matching the managed API.
#pragma once

#include <chaos/native_types.h>

extern "C" {

// ── XmlNode ──
void ChaosXmlNodeAppendChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) noexcept;
void ChaosXmlNodeInsertBefore(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child, CHAOS_IL2CPP_INTPTR ref_child) noexcept;
void ChaosXmlNodeInsertAfter(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child, CHAOS_IL2CPP_INTPTR ref_child) noexcept;
void ChaosXmlNodeReplaceChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child, CHAOS_IL2CPP_INTPTR old_child) noexcept;
void ChaosXmlNodeRemoveChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR old_child) noexcept;
void ChaosXmlNodePrependChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) noexcept;
CHAOS_IL2CPP_INT32 ChaosXmlNodeSupports(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR feature, CHAOS_IL2CPP_INTPTR version) noexcept;
void ChaosXmlNodeRemoveAll(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeGetEnumerator(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeClone(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeCloneNode(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 deep) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeGetNamespaceOfPrefix(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeGetPrefixOfNamespace(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ns_uri) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeCreateNavigator(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
void ChaosXmlNodeWriteTo(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) noexcept;
void ChaosXmlNodeWriteContentTo(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) noexcept;

// ── XmlDocument ──
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement2(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix, CHAOS_IL2CPP_INTPTR local_name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement3(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix, CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateAttribute(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateAttribute2(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix, CHAOS_IL2CPP_INTPTR local_name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateAttribute3(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix, CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentGetElementsByTagName2(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR local_name, CHAOS_IL2CPP_INTPTR ns) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateNode(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node_type);
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateNodeStr(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node_type_str, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR ns_uri) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateProcessingInstruction(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR data) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateDocumentType(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR pubid, CHAOS_IL2CPP_INTPTR sysid, CHAOS_IL2CPP_INTPTR subset) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateXmlDeclaration(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR version, CHAOS_IL2CPP_INTPTR encoding, CHAOS_IL2CPP_INTPTR standalone) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateEntityReference(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;
void ChaosXmlDocumentLoadXml(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR xml) noexcept;
void ChaosXmlDocumentLoadStream(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentSaveStream(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentGetElementsByTagName(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateNavigator(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentReadNode(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR reader) noexcept;
void ChaosXmlDocumentValidate(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR validation_event) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentImportNode(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node, CHAOS_IL2CPP_INT32 deep) noexcept;

// ── XmlCharacterData ──
void ChaosXmlCharacterDataReplaceData(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count, CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlCharacterDataSubstring(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosXmlCharacterDataInsertData(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INTPTR str) noexcept;
void ChaosXmlCharacterDataDeleteData(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosXmlCharacterDataAppendData(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR str) noexcept;

// ── XmlElement ──
void ChaosXmlElementSetAttribute(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosXmlElementSetAttributeNode(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR attr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlElementGetElementsByTagName(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlElementCloneNode(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 deep) noexcept;
void ChaosXmlElementWriteTo(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) noexcept;

// ── XmlAttribute ──
void ChaosXmlAttributeAppendChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) noexcept;
void ChaosXmlAttributePrependChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) noexcept;

// ── XmlAttributeCollection / XmlNodeList / XmlNamedNodeMap ──
CHAOS_IL2CPP_INTPTR ChaosXmlAttributeCollectionSetNamedItem(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) noexcept;
void ChaosXmlAttributeCollectionAppend(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) noexcept;
void ChaosXmlAttributeCollectionPrepend(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNodeListItem(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNamedNodeMapGetNamedItem(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNamedNodeMapSetNamedItem(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) noexcept;

// ── XmlNamespaceManager ──
void ChaosXmlNamespaceManagerAddNamespace(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix, CHAOS_IL2CPP_INTPTR uri) noexcept;
void ChaosXmlNamespaceManagerRemoveNamespace(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix) noexcept;
CHAOS_IL2CPP_INTPTR ChaosXmlNamespaceManagerGetEnumerator(CHAOS_IL2CPP_INTPTR this_ptr) noexcept;

}  // extern "C"
