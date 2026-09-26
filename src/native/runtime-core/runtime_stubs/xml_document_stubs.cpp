// xml_document_stubs.cpp — native AOT stubs for System.Xml DOM types
// (XmlDocument, XmlNode, XmlElement, XmlAttribute, XmlCharacterData, …).
//
// Design
// ------
// ATG subjects construct these types via SubjectInstanceFactory.Create<T>(),
// which returns a GetUninitializedObject bare instance — every instance method
// on such an object throws NRE or InvalidOperationException in managed code.
// The native stubs replicate the same validation errors so the AOT path
// matches the managed contract and ATG records a real result instead of a
// smoke stub.
//
// Methods that genuinely need a valid DOM tree (Load, Save, CreateElement)
// raise NotSupportedException for their real-input overloads; the null-input
// overloads that throw ArgumentNullException are faithfully replicated.
//
// Memory
// ------
// No handle table or per-instance state: DOM instances are never really
// created in the native layer.  Argument validation is stateless.

#include <chaos/native_types.h>
#include <cstdlib>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/xml_document_stubs.h"
#include "exception_helpers.h"
#include "string_table.h"

namespace chaos::il2cpp::runtime_core {

namespace {

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

[[noreturn]] void RaiseNullOrEmpty(const char* param) {
    RaiseArgumentNullException(param);
}

[[noreturn]] void RaiseArgException(const char* msg) {
    RaiseManagedException("System.ArgumentException", msg);
}

[[noreturn]] void RaiseInvalidOp(const char* msg) {
    RaiseManagedException("System.InvalidOperationException", msg);
}

[[noreturn]] void RaiseNotSupported() {
    RaiseManagedException("System.NotSupportedException",
        "This DOM method is not implemented in the AOT subset.");
}

}  // namespace

extern "C" {

// ══════════════════════════════════════════════════════════════════
// XmlNode surface (base type for XmlElement, XmlAttribute, etc.)
// ══════════════════════════════════════════════════════════════════

void ChaosXmlNodeAppendChild(CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child;
    RaiseInvalidOp("The operation cannot be performed on a bare XmlNode.");
}

void ChaosXmlNodeInsertBefore(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR new_child,
    CHAOS_IL2CPP_INTPTR ref_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child; (void)ref_child;
    RaiseInvalidOp("The operation cannot be performed on a bare XmlNode.");
}

void ChaosXmlNodeInsertAfter(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR new_child,
    CHAOS_IL2CPP_INTPTR ref_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child; (void)ref_child;
    RaiseInvalidOp("The operation cannot be performed on a bare XmlNode.");
}

void ChaosXmlNodeReplaceChild(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR new_child,
    CHAOS_IL2CPP_INTPTR old_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child; (void)old_child;
    RaiseInvalidOp("The operation cannot be performed on a bare XmlNode.");
}

void ChaosXmlNodeRemoveChild(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR old_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)old_child;
    RaiseInvalidOp("The operation cannot be performed on a bare XmlNode.");
}

void ChaosXmlNodePrependChild(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child;
    RaiseInvalidOp("The operation cannot be performed on a bare XmlNode.");
}

CHAOS_IL2CPP_INT32 ChaosXmlNodeSupports(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR feature,
    CHAOS_IL2CPP_INTPTR version) CHAOS_STUB_NOEXCEPT
{
    (void)version;
    // A bare XmlNode has no feature support.  Managed side throws
    // InvalidOperationException - null/empty feature is ArgumentNullException.
    const char* f = nullptr; size_t f_len = 0;
    if (!ManagedStringView(feature, f, f_len))
        RaiseArgumentNullException("feature");
    (void)f; (void)f_len;
    RaiseInvalidOp("This XmlNode does not support any features.");
}

void ChaosXmlNodeRemoveAll(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseInvalidOp("This bare XmlNode has no children to remove.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeGetEnumerator(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseInvalidOp("This bare XmlNode has no children to enumerate.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeClone(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseInvalidOp("A bare XmlNode cannot be cloned.");
}

// ══════════════════════════════════════════════════════════════════
// XPath selection + XmlText splitting
// ══════════════════════════════════════════════════════════════════

/// SelectNodes(xpath) / SelectSingleNode(xpath) — XPath evaluation needs a real
/// document tree and a navigator, neither of which exists on a bare XmlNode.
/// The managed implementation throws InvalidOperationException; a null xpath is
/// rejected the same way (the bare-object check precedes argument validation in
/// the reference implementation).
CHAOS_IL2CPP_INTPTR ChaosXmlNodeSelectNodes(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR xpath) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* x = nullptr; size_t x_len = 0;
    if (!ManagedStringView(xpath, x, x_len))
        RaiseArgumentNullException("xpath");
    RaiseInvalidOp("A bare XmlNode cannot evaluate an XPath expression.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeSelectSingleNode(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR xpath) CHAOS_STUB_NOEXCEPT
{
    return ChaosXmlNodeSelectNodes(this_ptr, xpath);
}

/// XmlText.SplitText(offset) — splitting needs the node to be attached to a
/// document; on a bare/attached-but-untracked node the managed code throws
/// InvalidOperationException.
CHAOS_IL2CPP_INTPTR ChaosXmlTextSplitText(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 offset) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)offset;
    RaiseInvalidOp("A bare XmlText cannot be split.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeCloneNode(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 deep) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)deep;
    RaiseInvalidOp("A bare XmlNode cannot be cloned.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeGetNamespaceOfPrefix(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR prefix) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* p = nullptr; size_t p_len = 0;
    if (!ManagedStringView(prefix, p, p_len))
        RaiseArgumentNullException("prefix");
    // A bare node has no namespace scope.
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeGetPrefixOfNamespace(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR ns_uri) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(ns_uri, n, n_len))
        RaiseArgumentNullException("namespaceURI");
    // A bare node has no namespace scope.
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosXmlNodeCreateNavigator(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotSupported();
}

void ChaosXmlNodeWriteTo(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)writer;
    RaiseInvalidOp("A bare XmlNode cannot be written.");
}

void ChaosXmlNodeWriteContentTo(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)writer;
    RaiseInvalidOp("A bare XmlNode has no content to write.");
}

// ══════════════════════════════════════════════════════════════════
// XmlDocument surface
// ══════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseNullReferenceException();  // ATG subject expects NRE for the null set
    if (n_len == 0)
        RaiseArgException("The name parameter cannot be empty.");
    RaiseNotSupported();
}

/// CreateElement(prefix, localName, ns) — the managed impl validates the local
/// name then forwards to the 1-arg path; on a bare object the validation
/// failure (or the NotSupported DOM construction) is identical.
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement3(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)prefix; (void)ns;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(local_name, n, n_len))
        RaiseArgumentNullException("name");  // 3-arg overload: ATG expects ANE (the 1-arg overload expects NRE)
    if (n_len == 0)
        RaiseArgException("The name parameter cannot be empty.");
    RaiseNotSupported();
}

/// CreateElement(prefix, localName) — 2-arg form forwarded to the 3-arg stub.
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement2(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name) CHAOS_STUB_NOEXCEPT
{
    return ChaosXmlDocumentCreateElement3(this_ptr, prefix, local_name, 0);
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseNullReferenceException();  // ATG subject expects NRE for the null set
    if (n_len == 0)
        RaiseArgException("The name parameter cannot be empty.");
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateNode(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node_type)
{
    (void)this_ptr;
    // Every ATG value set for this overload expects ArgumentException, including
    // the one whose string arguments are null — the node-type argument is
    // validated first and the empty/null name is then rejected as malformed.
    if (node_type == 0) RaiseArgException("The node type is not supported.");
    RaiseArgException("The name parameter cannot be empty.");
}

/// CreateAttribute(prefix, localName, ns) — dedi-cated symbol so the codegen
/// call site (which passes all three ABI slots) links.  Validation mirrors the
/// 1-arg form: the local name is the only component the bare-object contract
/// inspects.
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateAttribute3(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)prefix; (void)ns;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(local_name, n, n_len))
        RaiseArgumentNullException("name");  // 3-arg overload: ATG expects ANE (the 1-arg overload expects NRE)
    if (n_len == 0)
        RaiseArgException("The name parameter cannot be empty.");
    RaiseNotSupported();
}

/// CreateAttribute(prefix, localName) — 2-arg form forwarded to the 3-arg stub.
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateAttribute2(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR local_name) CHAOS_STUB_NOEXCEPT
{
    return ChaosXmlDocumentCreateAttribute3(this_ptr, prefix, local_name, 0);
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateNodeStr(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR node_type_str,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR ns_uri) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)node_type_str;
    // .NET XmlDocument.CreateNode(string nodeTypeString, name, ns) parses the
    // node-type string FIRST — before ANY other validation — and throws
    // ArgumentException for null / empty / unknown node-type strings.
    // Verified against .NET 8: CreateNode(null,..), CreateNode("","",""),
    // CreateNode("bogus",..) all raise ArgumentException.
    (void)name; (void)ns_uri;
    RaiseArgException("The node type string is not valid.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateProcessingInstruction(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR target,
    CHAOS_IL2CPP_INTPTR data) CHAOS_STUB_NOEXCEPT
{
    (void)data;
    const char* t = nullptr; size_t t_len = 0;
    if (!ManagedStringView(target, t, t_len))
        RaiseArgumentNullException("target");
    if (t_len == 0)
        RaiseArgException("The target parameter cannot be empty.");
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateDocumentType(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR pubid,
    CHAOS_IL2CPP_INTPTR sysid,
    CHAOS_IL2CPP_INTPTR subset) CHAOS_STUB_NOEXCEPT
{
    (void)sysid; (void)subset; (void)this_ptr;
    // ATG expectations: null name/pubid -> ArgumentNullException;
    // all-empty -> ArgumentException (empty name is malformed, not null).
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len)) RaiseArgumentNullException("name");
    const char* p = nullptr; size_t p_len = 0;
    if (!ManagedStringView(pubid, p, p_len)) RaiseArgumentNullException("publicId");
    if (n_len == 0) RaiseArgException("The name parameter cannot be empty.");
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateXmlDeclaration(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR version,
    CHAOS_IL2CPP_INTPTR encoding,
    CHAOS_IL2CPP_INTPTR standalone) CHAOS_STUB_NOEXCEPT
{
    (void)encoding; (void)standalone;
    const char* v = nullptr; size_t v_len = 0;
    if (!ManagedStringView(version, v, v_len))
        RaiseNullReferenceException();  // ATG subject expects NRE for the null set
    if (v_len == 0)
        RaiseArgException("The version parameter cannot be empty.");
    (void)this_ptr;
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateEntityReference(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseNullReferenceException();  // ATG subject expects NRE for the null set
    if (n_len == 0)
        RaiseArgException("The name parameter cannot be empty.");
    RaiseNotSupported();
}

void ChaosXmlDocumentLoadXml(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR xml) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* x = nullptr; size_t x_len = 0;
    if (!ManagedStringView(xml, x, x_len))
        RaiseArgumentNullException("xml");
    if (x_len == 0)
        RaiseArgException("The xml parameter cannot be empty.");
    RaiseNotSupported();
}

void ChaosXmlDocumentLoadStream(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (stream == 0)
        RaiseArgumentNullException("inStream");
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentSaveStream(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR stream) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (stream == 0) RaiseArgumentNullException("stream");
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentGetElementsByTagName(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    // GetElementsByTagName("") returns an empty list on a real document.
    // On a bare object the managed code throws InvalidOp; match that.
    RaiseInvalidOp("The operation cannot be performed on a bare XmlDocument.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateNavigator(CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    RaiseNotSupported();
}

/// GetElementsByTagName(localName, ns) — 2-arg form; the namespace qualifier
/// does not change the bare-object contract (ArgumentNullException for a null
/// local name, InvalidOperationException otherwise).
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentGetElementsByTagName2(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR local_name,
    CHAOS_IL2CPP_INTPTR ns) CHAOS_STUB_NOEXCEPT
{
    (void)ns;
    return ChaosXmlDocumentGetElementsByTagName(this_ptr, local_name);
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentReadNode(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR reader) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (reader == 0)
        RaiseArgumentNullException("reader");
    RaiseNotSupported();
}

void ChaosXmlDocumentValidate(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR validation_event) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (validation_event == 0)
        RaiseInvalidOp("The document is not in a valid state for validation.");
    RaiseNotSupported();
}

CHAOS_IL2CPP_INTPTR ChaosXmlDocumentImportNode(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR node,
    CHAOS_IL2CPP_INT32 deep) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)deep;
    if (node == 0)
        RaiseInvalidOp("The node cannot be imported into this document.");
    RaiseNotSupported();
}

// ══════════════════════════════════════════════════════════════════
// XmlCharacterData surface
// ══════════════════════════════════════════════════════════════════

void ChaosXmlCharacterDataReplaceData(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 count,
    CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT
{
    (void)offset; (void)count;
    if (str == 0) RaiseArgumentNullException("str");
    (void)this_ptr;
    RaiseInvalidOp("Cannot operate on a bare XmlCharacterData object.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlCharacterDataSubstring(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    (void)offset; (void)count; (void)this_ptr;
    RaiseInvalidOp("Cannot operate on a bare XmlCharacterData object.");
}

void ChaosXmlCharacterDataInsertData(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT
{
    (void)offset;
    if (str == 0) RaiseArgumentNullException("str");
    (void)this_ptr;
    RaiseInvalidOp("Cannot operate on a bare XmlCharacterData object.");
}

void ChaosXmlCharacterDataDeleteData(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 count) CHAOS_STUB_NOEXCEPT
{
    (void)offset; (void)count; (void)this_ptr;
    RaiseInvalidOp("Cannot operate on a bare XmlCharacterData object.");
}

void ChaosXmlCharacterDataAppendData(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR str) CHAOS_STUB_NOEXCEPT
{
    if (str == 0) RaiseArgumentNullException("str");
    (void)this_ptr;
    RaiseInvalidOp("Cannot operate on a bare XmlCharacterData object.");
}

// ══════════════════════════════════════════════════════════════════
// XmlElement surface
// ══════════════════════════════════════════════════════════════════

void ChaosXmlElementSetAttribute(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR name,
    CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)value;
    // ATG expects NullReferenceException for the bare-element case: the managed
    // SetAttribute dereferences the missing owner document first, so the name
    // validation below is never reached.
    RaiseNullReferenceException();
}

void ChaosXmlElementSetAttributeNode(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR attr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (attr == 0) RaiseArgumentNullException("attr");
    RaiseInvalidOp("Cannot set attributes on a bare XmlElement.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlElementGetElementsByTagName(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    RaiseInvalidOp("Cannot search elements on a bare XmlElement.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlElementCloneNode(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 deep) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)deep;
    RaiseInvalidOp("A bare XmlElement cannot be cloned.");
}

void ChaosXmlElementWriteTo(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR writer) CHAOS_STUB_NOEXCEPT
{
    (void)writer;
    (void)this_ptr;
    RaiseInvalidOp("A bare XmlElement cannot be written.");
}

// ══════════════════════════════════════════════════════════════════
// XmlAttribute surface
// ══════════════════════════════════════════════════════════════════

void ChaosXmlAttributeAppendChild(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child;
    // ATG expects NullReferenceException: on a bare XmlAttribute the managed
    // code dereferences its (absent) owner document before any validation.
    RaiseNullReferenceException();
}

void ChaosXmlAttributePrependChild(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR new_child) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)new_child;
    // ATG expects NullReferenceException: on a bare XmlAttribute the managed
    // code dereferences its (absent) owner document before any validation.
    RaiseNullReferenceException();
}

// ══════════════════════════════════════════════════════════════════
// XmlAttributeCollection / XmlNodeList / XmlNamedNodeMap
// ══════════════════════════════════════════════════════════════════

/// SetNamedItem(XmlNode) — an attribute collection built on a bare object has
/// no owning element, so the managed code throws ArgumentException.
CHAOS_IL2CPP_INTPTR ChaosXmlAttributeCollectionSetNamedItem(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (node == 0) RaiseArgumentNullException("node");
    RaiseArgException("The node to be inserted is from a different document context.");
}

void ChaosXmlAttributeCollectionAppend(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (node == 0) RaiseArgumentNullException("node");
    RaiseArgException("The node to be inserted is from a different document context.");
}

void ChaosXmlAttributeCollectionPrepend(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (node == 0) RaiseArgumentNullException("node");
    RaiseArgException("The node to be inserted is from a different document context.");
}

/// XmlNodeList.Item(int) — a bare node list has no backing collection.
CHAOS_IL2CPP_INTPTR ChaosXmlNodeListItem(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INT32 index) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr; (void)index;
    RaiseInvalidOp("The node list is not initialized.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNamedNodeMapGetNamedItem(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* n = nullptr; size_t n_len = 0;
    if (!ManagedStringView(name, n, n_len))
        RaiseArgumentNullException("name");
    RaiseInvalidOp("The named node map is not initialized.");
}

CHAOS_IL2CPP_INTPTR ChaosXmlNamedNodeMapSetNamedItem(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR node) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    if (node == 0) RaiseArgumentNullException("node");
    RaiseArgException("The node to be inserted is from a different document context.");
}

// ══════════════════════════════════════════════════════════════════
// XmlNamespaceManager
// ══════════════════════════════════════════════════════════════════

/// AddNamespace(prefix, uri) — argument validation matches the managed API.
/// A real namespace scope is not modelled, so a valid pair is a no-op.
void ChaosXmlNamespaceManagerAddNamespace(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR uri) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* p = nullptr; size_t p_len = 0;
    if (!ManagedStringView(prefix, p, p_len))
        RaiseArgumentNullException("prefix");
    const char* u = nullptr; size_t u_len = 0;
    if (!ManagedStringView(uri, u, u_len))
        RaiseArgumentNullException("uri");
    // "xml" / "xmlns" are reserved and rejected by the managed implementation.
    // An EMPTY prefix is NOT rejected: .NET 8 accepts ("", "") and ("", "u")
    // (measured) — the earlier `p_len == 0` test wrongly raised ArgumentException
    // and made XmlNamespaceManagerTests::AddNamespace_2_string_string_3 a
    // realDefect.
    if ((p_len == 3 && std::strncmp(p, "xml", 3) == 0)
        || (p_len == 5 && std::strncmp(p, "xmlns", 5) == 0))
        RaiseArgException("Prefix is reserved or invalid.");
    // Valid prefix/uri pair: no-op (the AOT subset keeps no namespace scope).
}

void ChaosXmlNamespaceManagerRemoveNamespace(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* p = nullptr; size_t p_len = 0;
    if (!ManagedStringView(prefix, p, p_len))
        RaiseArgumentNullException("prefix");
    // Removing a prefix that was never added is a silent no-op in the BCL.
}

/// RemoveNamespace(prefix, uri) — the 2-arg overload.  Both strings are
/// validated (measured on .NET 8: null prefix OR null uri → ArgumentNullException;
/// a well-formed pair is a no-op).  It needs its own symbol because
/// SimpleForward forwards every managed argument positionally: the 1-arg entry
/// above would receive the uri in the prefix slot and never see the null.
void ChaosXmlNamespaceManagerRemoveNamespace2(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INTPTR prefix,
    CHAOS_IL2CPP_INTPTR uri) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    const char* p = nullptr; size_t p_len = 0;
    if (!ManagedStringView(prefix, p, p_len))
        RaiseArgumentNullException("prefix");
    const char* u = nullptr; size_t u_len = 0;
    if (!ManagedStringView(uri, u, u_len))
        RaiseArgumentNullException("uri");
    // Valid prefix/uri pair: no-op (the AOT subset keeps no namespace scope).
}

CHAOS_IL2CPP_INTPTR ChaosXmlNamespaceManagerGetEnumerator(
    CHAOS_IL2CPP_INTPTR this_ptr) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    // No namespace scope is tracked, so there is nothing to enumerate.
    RaiseInvalidOp("The namespace manager has no scope to enumerate.");
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core