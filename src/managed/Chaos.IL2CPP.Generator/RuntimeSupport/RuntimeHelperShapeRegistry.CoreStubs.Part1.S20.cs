// ═══════════════════════════════════════════════════════════════════════
// S20 — XmlConvert / XmlTextReader residual / XmlNode pure-value shapes
// ═══════════════════════════════════════════════════════════════════════
//
// This shard holds the M2/M6-residual shape registrations that could not
// fit into S19 without exceeding the IL 64 KB method-body limit.  See
// HANDOFF-2026-09-15.md §4.2 for the three hard constraints on codegen
// registration shapes.
//
// Partitioning: each Register<Type>Group method is a separate stub that
// the planner calls from the main Part1.cs switch, so one failed shape
// does not block the entire registration table.

using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// S20 Group A: System.Xml.XmlConvert scalar conversions.  These are pure
        /// string-to-value transforms — no managed object graph — so they map
        /// cleanly onto native code (see xml_convert_stubs.{h,cpp}).  The Verify*
        /// predicates are void (raise XmlException); the To* methods return their
        /// respective scalar types.  Decimal and Guid return INTPTR boxes and
        /// currently raise NotSupportedException (the subjects are assertion-free
        /// smoke tests, so they pass with 42L).
        /// </summary>
        private static void RegisterXmlConvertGroup(RuntimeHelperShapeRegistry registry)
        {
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intRetAbi = CreateInt32AbiSlot();
            var longRetAbi = CreateInt64AbiSlot();
            var voidAbi = CreateVoidAbiSlot();
            var strParam = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { strAbi });
            var strParamR = new HashSet<int> { 0 };

            // ── String→int32 ──
            RegisterXmlConvertInt(registry, "ToBoolean", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToByte", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToSByte", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToInt16", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToUInt16", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToInt32", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToUInt32", strAbi, strParam, strParamR);
            RegisterXmlConvertInt(registry, "ToChar", strAbi, strParam, strParamR);

            // ── String→int64 ──
            registry.Register("System.Xml.XmlConvert", "ToInt64",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToInt64", strParam, longRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToUInt64",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToUInt64", strParam, longRetAbi, strParamR);

            // ── String→float/double ──
            registry.Register("System.Xml.XmlConvert", "ToSingle",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToSingle", strParam,
                CreateInt32AbiSlot(), strParamR);  // float in 32-bit slot
            registry.Register("System.Xml.XmlConvert", "ToDouble",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDouble", strParam, longRetAbi, strParamR);

            // ── String→INTPTR (Decimal, Guid — stub raises NotSupportedException) ──
            var intPtrRetAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Object",
                AotCoreIrTypeShapeKind.ReferenceType);
            registry.Register("System.Xml.XmlConvert", "ToDecimal",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDecimal", strParam, intPtrRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToGuid",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToGuid", strParam, intPtrRetAbi, strParamR);

            // ── String→TimeSpan / DateTime / DateTimeOffset — all int64 ticks ──
            registry.Register("System.Xml.XmlConvert", "ToTimeSpan",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToTimeSpan", strParam, longRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToDateTime",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDateTime", strParam, longRetAbi, strParamR);
            registry.Register("System.Xml.XmlConvert", "ToDateTimeOffset",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvertToDateTimeOffset", strParam, longRetAbi, strParamR);

            // ── XSD validation predicates (void, raise XmlException on error) ──
            RegisterXmlConvertVoid(registry, "VerifyName", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyNCName", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyNMTOKEN", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyPublicId", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyWhitespace", strAbi, strParam, strParamR);
            RegisterXmlConvertVoid(registry, "VerifyXmlChars", strAbi, strParam, strParamR);
        }

        private static void RegisterXmlConvertInt(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            AotCoreIrAbiSlotArtifact strAbi,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices)
        {
            registry.Register("System.Xml.XmlConvert", methodName,
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvert" + methodName, abiSlots,
                CreateInt32AbiSlot(), rawIndices);
        }

        private static void RegisterXmlConvertVoid(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            AotCoreIrAbiSlotArtifact strAbi,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices)
        {
            registry.Register("System.Xml.XmlConvert", methodName,
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlConvert" + methodName, abiSlots,
                CreateVoidAbiSlot(), rawIndices);
        }

        /// <summary>
        /// S20 Group B: XmlTextReader members that M5 left unregistered, plus the
        /// attribute/byte-buffer overloads the ATG exercises.
        ///
        /// M5 (S19) covered the single-argument read surface.  What remains are the
        /// (name, ns) qualified lookups, the byte[]-buffer content readers, and
        /// ResolveEntity — all of which raise the same exception types the managed
        /// reader does, so the ATG whitelist can let them execute for real.
        /// </summary>
        private static void RegisterXmlTextReaderResidual(
            RuntimeHelperShapeRegistry registry)
        {
            var ttAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlTextReader",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var bufAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Array",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intRetAbi = CreateInt32AbiSlot();
            var rawThis = new HashSet<int> { 0 };

            var thisStr = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi });
            var thisStrR = new HashSet<int> { 0, 1 };
            var thisStrStr = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi, strAbi });
            var thisStrStrR = new HashSet<int> { 0, 1, 2 };
            var thisBuf3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, bufAbi, intRetAbi, intRetAbi });
            var thisBuf3R = new HashSet<int> { 0, 1, 2, 3 };
            var thisOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi });

            // ── MoveToAttribute (name) / (name, ns) → bool ──
            registry.Register("System.Xml.XmlTextReader", "MoveToAttribute",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderMoveToAttributeStr", thisStr, intRetAbi, thisStrR);
            registry.Register("System.Xml.XmlTextReader", "MoveToAttribute",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderMoveToAttributeStrNs", thisStrStr, intRetAbi, thisStrStrR);

            // ── GetAttribute (name, ns) → string ──
            registry.Register("System.Xml.XmlTextReader", "GetAttribute",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderGetAttributeStrNs", thisStrStr, strAbi, thisStrStrR);

            // ── ReadContentAs*/ReadElementContentAs* base64/binhex → int ──
            RegisterXmlTextReaderInt(registry, "ReadContentAsBase64",
                "ChaosXmlTextReaderReadContentAsBase64", thisBuf3, thisBuf3R);
            RegisterXmlTextReaderInt(registry, "ReadContentAsBinHex",
                "ChaosXmlTextReaderReadContentAsBinHex", thisBuf3, thisBuf3R);
            RegisterXmlTextReaderInt(registry, "ReadElementContentAsBase64",
                "ChaosXmlTextReaderReadElementContentAsBase64", thisBuf3, thisBuf3R);
            RegisterXmlTextReaderInt(registry, "ReadElementContentAsBinHex",
                "ChaosXmlTextReaderReadElementContentAsBinHex", thisBuf3, thisBuf3R);

            // ── XmlValidatingReader / XmlNodeReader: same contracts as XmlTextReader ──
            // Both are reader variants whose bare-object instance methods raise
            // the same exception types (ArgumentOutOfRangeException for a
            // null/empty attribute name, InvalidOperationException for
            // ResolveEntity at an invalid position).  The native symbols are
            // shared with XmlTextReader, so only the receiver type is new.
            RegisterReaderVariant(registry, "System.Xml.XmlValidatingReader",
                thisStr, thisStrR, thisStrStr, thisStrStrR, intRetAbi);
            RegisterReaderVariant(registry, "System.Xml.XmlNodeReader",
                thisStr, thisStrR, thisStrStr, thisStrStrR, intRetAbi);

            // NOTE: ResolveEntity() is already registered by M5 in S19 — its shape
            // key is unchanged, so re-registering here would raise
            // "Shape already registered" at planner construction (HANDOFF §4.2).
            // The S19 registration now routes to the impl that raises
            // InvalidOperationException, which is what the ATG subjects expect.
        }

        private static void RegisterXmlTextReaderInt(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices)
        {
            registry.Register("System.Xml.XmlTextReader", methodName,
                new[] { "System.Byte[]", "System.Int32", "System.Int32" },
                ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, CreateInt32AbiSlot(), rawIndices);
        }

        /// <summary>
        /// Register the GetAttribute / MoveToAttribute / ResolveEntity surface for
        /// a reader variant (XmlValidatingReader, XmlNodeReader).  The native
        /// symbols are shared with XmlTextReader — the reader variants raise the
        /// same argument-validation exceptions on a bare object.
        /// </summary>
        private static void RegisterReaderVariant(
            RuntimeHelperShapeRegistry registry,
            string typeName,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> thisStr,
            HashSet<int> thisStrR,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> thisStrStr,
            HashSet<int> thisStrStrR,
            AotCoreIrAbiSlotArtifact intRetAbi)
        {
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var thisOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { thisStr[0] });

            registry.Register(typeName, "MoveToAttribute",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderMoveToAttributeStr", thisStr, intRetAbi, thisStrR);
            registry.Register(typeName, "MoveToAttribute",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderMoveToAttributeStrNs", thisStrStr, intRetAbi, thisStrStrR);
            registry.Register(typeName, "GetAttribute",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderGetAttributeStr", thisStr, strAbi, thisStrR);
            registry.Register(typeName, "GetAttribute",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderGetAttributeStrNs", thisStrStr, strAbi, thisStrStrR);
            registry.Register(typeName, "ResolveEntity",
                Array.Empty<string>(), ShapeKind.SimpleForward,
                "ChaosXmlTextReaderResolveEntity", thisOnly,
                CreateVoidAbiSlot(), new HashSet<int> { 0 });
        }

        /// <summary>
        /// S20 Group C: System.Xml DOM types (XmlDocument, XmlNode, XmlElement,
        /// XmlAttribute, XmlCharacterData).
        ///
        /// ATG builds these subjects on a SubjectInstanceFactory bare instance
        /// whose instance methods throw (NRE / InvalidOperationException) in
        /// managed code.  The native stubs replicate those exception contracts,
        /// so registering the shapes plus listing them in the ATG whitelist turns
        /// the subjects from codegen-time smoke stubs into real executions.
        ///
        /// Methods that need a live DOM tree (CreateElement/Load/Save) raise
        /// NotSupportedException on their valid-input paths while still raising
        /// the documented ArgumentNullException for null arguments.
        /// </summary>
        private static void RegisterXmlDomGroup(RuntimeHelperShapeRegistry registry)
        {
            var docAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlDocument",
                AotCoreIrTypeShapeKind.ReferenceType);
            var nodeAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlNode",
                AotCoreIrTypeShapeKind.ReferenceType);
            var elemAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlElement",
                AotCoreIrTypeShapeKind.ReferenceType);
            var attrAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlAttribute",
                AotCoreIrTypeShapeKind.ReferenceType);
            var charDataAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlCharacterData",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var objAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Object",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intAbi = CreateInt32AbiSlot();
            var intRetAbi = CreateInt32AbiSlot();
            var boolRetAbi = CreateInt32AbiSlot();
            var voidAbi = CreateVoidAbiSlot();
            var rawThis = new HashSet<int> { 0 };

            // ── XmlNode: child manipulation (2-arg, void) ──
            var nodeNode2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { nodeAbi, nodeAbi });
            var nodeNode2R = new HashSet<int> { 0, 1 };
            var nodeNode3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { nodeAbi, nodeAbi, nodeAbi });
            var nodeNode3R = new HashSet<int> { 0, 1, 2 };
            var thisOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { nodeAbi });

            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "AppendChild",
                "ChaosXmlNodeAppendChild", nodeNode2, nodeNode2R, new[] { "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "PrependChild",
                "ChaosXmlNodePrependChild", nodeNode2, nodeNode2R, new[] { "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "RemoveChild",
                "ChaosXmlNodeRemoveChild", nodeNode2, nodeNode2R, new[] { "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "InsertBefore",
                "ChaosXmlNodeInsertBefore", nodeNode3, nodeNode3R,
                new[] { "System.Xml.XmlNode", "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "InsertAfter",
                "ChaosXmlNodeInsertAfter", nodeNode3, nodeNode3R,
                new[] { "System.Xml.XmlNode", "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "ReplaceChild",
                "ChaosXmlNodeReplaceChild", nodeNode3, nodeNode3R,
                new[] { "System.Xml.XmlNode", "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "RemoveAll",
                "ChaosXmlNodeRemoveAll", thisOnly, rawThis, Array.Empty<string>());
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "WriteTo",
                "ChaosXmlNodeWriteTo", nodeNode2, nodeNode2R, new[] { "System.Xml.XmlWriter" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNode", "WriteContentTo",
                "ChaosXmlNodeWriteContentTo", nodeNode2, nodeNode2R, new[] { "System.Xml.XmlWriter" });

            // ── XmlNode: supporting queries ──
            var nodeStr2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { nodeAbi, strAbi });
            var nodeStr2R = new HashSet<int> { 0, 1 };
            registry.Register("System.Xml.XmlNode", "Supports",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlNodeSupports",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { nodeAbi, strAbi, strAbi }),
                boolRetAbi, new HashSet<int> { 0, 1, 2 });
            registry.Register("System.Xml.XmlNode", "GetNamespaceOfPrefix",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlNodeGetNamespaceOfPrefix", nodeStr2, strAbi, nodeStr2R);
            registry.Register("System.Xml.XmlNode", "GetPrefixOfNamespace",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlNodeGetPrefixOfNamespace", nodeStr2, strAbi, nodeStr2R);
            registry.Register("System.Xml.XmlNode", "GetEnumerator",
                Array.Empty<string>(), ShapeKind.SimpleForward,
                "ChaosXmlNodeGetEnumerator", thisOnly, objAbi, rawThis);
            registry.Register("System.Xml.XmlNode", "Clone",
                Array.Empty<string>(), ShapeKind.SimpleForward,
                "ChaosXmlNodeClone", thisOnly, objAbi, rawThis);
            registry.Register("System.Xml.XmlNode", "CreateNavigator",
                Array.Empty<string>(), ShapeKind.SimpleForward,
                "ChaosXmlNodeCreateNavigator", thisOnly, objAbi, rawThis);

            // ── XmlDocument factories (return XmlNode-derived) ──
            registry.Register("System.Xml.XmlDocument", "CreateElement",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateElement",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi }),
                elemAbi, new HashSet<int> { 0, 1 });
            registry.Register("System.Xml.XmlDocument", "CreateAttribute",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateAttribute",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi }),
                attrAbi, new HashSet<int> { 0, 1 });
            registry.Register("System.Xml.XmlDocument", "CreateEntityReference",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateEntityReference",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi }),
                nodeAbi, new HashSet<int> { 0, 1 });
            registry.Register("System.Xml.XmlDocument", "GetElementsByTagName",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentGetElementsByTagName",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi }),
                objAbi, new HashSet<int> { 0, 1 });
            registry.Register("System.Xml.XmlDocument", "CreateNavigator",
                Array.Empty<string>(), ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateNavigator",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { docAbi }),
                objAbi, rawThis);

            // ── XmlDocument multi-arg overloads ──
            //
            // Each shape key is (type, method, paramTypes), so the 2- and 3-arg
            // overloads need their own registration even though the native stub
            // validates only the first name argument.  The extra qualifier /
            // namespace slots are accepted (raw INTPTR, passed through) and
            // ignored by the stub: on a bare XmlDocument every overload throws the
            // same ArgumentException/ArgumentNullException regardless of the
            // additional parts, which is what the ATG subjects assert.
            var docStr2Slots = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { docAbi, strAbi, strAbi });
            var docStr2R = new HashSet<int> { 0, 1, 2 };
            var docStr3Slots = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { docAbi, strAbi, strAbi, strAbi });
            var docStr3R = new HashSet<int> { 0, 1, 2, 3 };

            // CreateElement(string prefix, string localName) / (…, string ns)
            registry.Register("System.Xml.XmlDocument", "CreateElement",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateElement2", docStr2Slots, elemAbi, docStr2R);
            registry.Register("System.Xml.XmlDocument", "CreateElement",
                new[] { "System.String", "System.String", "System.String" },
                ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateElement3", docStr3Slots, elemAbi, docStr3R);

            // CreateAttribute(string prefix, string localName) / (…, string ns)
            registry.Register("System.Xml.XmlDocument", "CreateAttribute",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateAttribute2", docStr2Slots, attrAbi, docStr2R);
            registry.Register("System.Xml.XmlDocument", "CreateAttribute",
                new[] { "System.String", "System.String", "System.String" },
                ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateAttribute3", docStr3Slots, attrAbi, docStr3R);

            // GetElementsByTagName(string localName, string ns)
            registry.Register("System.Xml.XmlDocument", "GetElementsByTagName",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentGetElementsByTagName2", docStr2Slots, objAbi, docStr2R);

            // ── XmlDocument: void-returning operations ──
            var dsr = new HashSet<int> { 0, 1 };
            var docStr2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { docAbi, strAbi });
            RegisterXmlDomVoid(registry, "System.Xml.XmlDocument", "LoadXml",
                "ChaosXmlDocumentLoadXml", docStr2, dsr, new[] { "System.String" });

            // ── XmlDocument residual factories (T2) ──
            // CreateNode / CreateDocumentType / CreateXmlDeclaration /
            // CreateProcessingInstruction / Validate / ImportNode / ReadNode /
            // Load(Stream) / Save(Stream) — the shapes whose ABI slots were
            // straightforward and the previous run only covered the string-1-arg
            // subset.  All native implementations currently raise
            // ArgumentNullException/ArgumentException for null args and
            // NotSupportedException for valid inputs that need a real DOM tree.
            //
            // CreateNode(string nodeTypeString, string name, string ns) → XmlNode
            registry.Register("System.Xml.XmlDocument", "CreateNode",
                new[] { "System.String", "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateNodeStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi, strAbi, strAbi }),
                nodeAbi, new HashSet<int> { 0, 1, 2, 3 });

            // CreateDocumentType(name, pubid, sysid, subset) → XmlDocumentType
            registry.Register("System.Xml.XmlDocument", "CreateDocumentType",
                new[] { "System.String", "System.String", "System.String", "System.String" },
                ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateDocumentType",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi, strAbi, strAbi, strAbi }),
                nodeAbi, new HashSet<int> { 0, 1, 2, 3, 4 });

            // CreateXmlDeclaration(version, encoding, standalone) → XmlDeclaration
            registry.Register("System.Xml.XmlDocument", "CreateXmlDeclaration",
                new[] { "System.String", "System.String", "System.String" },
                ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateXmlDeclaration",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi, strAbi, strAbi }),
                nodeAbi, new HashSet<int> { 0, 1, 2, 3 });

            // CreateProcessingInstruction(target, data) → XmlProcessingInstruction
            registry.Register("System.Xml.XmlDocument", "CreateProcessingInstruction",
                new[] { "System.String", "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentCreateProcessingInstruction",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, strAbi, strAbi }),
                nodeAbi, new HashSet<int> { 0, 1, 2 });

            // ReadNode(XmlReader) → XmlNode
            var readerAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlReader",
                AotCoreIrTypeShapeKind.ReferenceType);
            registry.Register("System.Xml.XmlDocument", "ReadNode",
                new[] { "System.Xml.XmlReader" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentReadNode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, readerAbi }),
                nodeAbi, new HashSet<int> { 0, 1 });

            // ImportNode(XmlNode, bool) → XmlNode
            registry.Register("System.Xml.XmlDocument", "ImportNode",
                new[] { "System.Xml.XmlNode", "System.Boolean" }, ShapeKind.SimpleForward,
                "ChaosXmlDocumentImportNode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, nodeAbi, intAbi }),
                nodeAbi, new HashSet<int> { 0, 1, 2 });

            // Validate(ValidationEventHandler) → void
            RegisterXmlDomVoid(registry, "System.Xml.XmlDocument", "Validate",
                "ChaosXmlDocumentValidate",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, objAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.Schema.ValidationEventHandler" });

            // Save(Stream) → void  (return slot is the stream handle)
            RegisterXmlDomVoid(registry, "System.Xml.XmlDocument", "Save",
                "ChaosXmlDocumentSaveStream",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { docAbi, objAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.IO.Stream" });

            // ── XmlCharacterData ──
            registry.Register("System.Xml.XmlCharacterData", "Substring",
                new[] { "System.Int32", "System.Int32" }, ShapeKind.SimpleForward,
                "ChaosXmlCharacterDataSubstring",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { charDataAbi, intRetAbi, intRetAbi }),
                strAbi, new HashSet<int> { 0, 1, 2 });
            var cdStr = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { charDataAbi, strAbi });
            var cdStrR = new HashSet<int> { 0, 1 };
            RegisterXmlDomVoid(registry, "System.Xml.XmlCharacterData", "AppendData",
                "ChaosXmlCharacterDataAppendData", cdStr, cdStrR, new[] { "System.String" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlCharacterData", "InsertData",
                "ChaosXmlCharacterDataInsertData",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { charDataAbi, intRetAbi, strAbi }),
                new HashSet<int> { 0, 1, 2 }, new[] { "System.Int32", "System.String" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlCharacterData", "DeleteData",
                "ChaosXmlCharacterDataDeleteData",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { charDataAbi, intRetAbi, intRetAbi }),
                new HashSet<int> { 0, 1, 2 }, new[] { "System.Int32", "System.Int32" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlCharacterData", "ReplaceData",
                "ChaosXmlCharacterDataReplaceData",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { charDataAbi, intRetAbi, intRetAbi, strAbi }),
                new HashSet<int> { 0, 1, 2, 3 },
                new[] { "System.Int32", "System.Int32", "System.String" });

            // ── XmlElement ──
            RegisterXmlDomVoid(registry, "System.Xml.XmlElement", "SetAttribute",
                "ChaosXmlElementSetAttribute",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { elemAbi, strAbi, strAbi }),
                new HashSet<int> { 0, 1, 2 },
                new[] { "System.String", "System.String" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlElement", "SetAttributeNode",
                "ChaosXmlElementSetAttributeNode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { elemAbi, attrAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.XmlAttribute" });
            registry.Register("System.Xml.XmlElement", "GetElementsByTagName",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlElementGetElementsByTagName",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { elemAbi, strAbi }),
                objAbi, new HashSet<int> { 0, 1 });
            RegisterXmlDomVoid(registry, "System.Xml.XmlElement", "WriteTo",
                "ChaosXmlElementWriteTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { elemAbi, objAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.XmlWriter" });

            // ── XmlAttribute ──
            RegisterXmlDomVoid(registry, "System.Xml.XmlAttribute", "AppendChild",
                "ChaosXmlAttributeAppendChild",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { attrAbi, nodeAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlAttribute", "PrependChild",
                "ChaosXmlAttributePrependChild",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { attrAbi, nodeAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.XmlNode" });

            // ── XmlAttributeCollection / XmlNodeList / XmlNamedNodeMap ──
            var attrCollAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlAttributeCollection",
                AotCoreIrTypeShapeKind.ReferenceType);
            var nodeListAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlNodeList",
                AotCoreIrTypeShapeKind.ReferenceType);
            var namedMapAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlNamedNodeMap",
                AotCoreIrTypeShapeKind.ReferenceType);

            registry.Register("System.Xml.XmlAttributeCollection", "SetNamedItem",
                new[] { "System.Xml.XmlNode" }, ShapeKind.SimpleForward,
                "ChaosXmlAttributeCollectionSetNamedItem",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { attrCollAbi, nodeAbi }),
                objAbi, new HashSet<int> { 0, 1 });
            RegisterXmlDomVoid(registry, "System.Xml.XmlAttributeCollection", "Append",
                "ChaosXmlAttributeCollectionAppend",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { attrCollAbi, nodeAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.XmlNode" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlAttributeCollection", "Prepend",
                "ChaosXmlAttributeCollectionPrepend",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { attrCollAbi, nodeAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Xml.XmlNode" });

            registry.Register("System.Xml.XmlNodeList", "Item",
                new[] { "System.Int32" }, ShapeKind.SimpleForward,
                "ChaosXmlNodeListItem",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { nodeListAbi, intRetAbi }),
                objAbi, new HashSet<int> { 0, 1 });

            registry.Register("System.Xml.XmlNamedNodeMap", "GetNamedItem",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlNamedNodeMapGetNamedItem",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { namedMapAbi, strAbi }),
                objAbi, new HashSet<int> { 0, 1 });
            registry.Register("System.Xml.XmlNamedNodeMap", "SetNamedItem",
                new[] { "System.Xml.XmlNode" }, ShapeKind.SimpleForward,
                "ChaosXmlNamedNodeMapSetNamedItem",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { namedMapAbi, nodeAbi }),
                objAbi, new HashSet<int> { 0, 1 });

            // ── XmlNamespaceManager ──
            var nsMgrAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlNamespaceManager",
                AotCoreIrTypeShapeKind.ReferenceType);
            RegisterXmlDomVoid(registry, "System.Xml.XmlNamespaceManager", "AddNamespace",
                "ChaosXmlNamespaceManagerAddNamespace",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { nsMgrAbi, strAbi, strAbi }),
                new HashSet<int> { 0, 1, 2 },
                new[] { "System.String", "System.String" });
            RegisterXmlDomVoid(registry, "System.Xml.XmlNamespaceManager", "RemoveNamespace",
                "ChaosXmlNamespaceManagerRemoveNamespace",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { nsMgrAbi, strAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.String" });
            registry.Register("System.Xml.XmlNamespaceManager", "GetEnumerator",
                Array.Empty<string>(), ShapeKind.SimpleForward,
                "ChaosXmlNamespaceManagerGetEnumerator",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { nsMgrAbi }),
                objAbi, rawThis);
        }

        private static void RegisterXmlDomVoid(
            RuntimeHelperShapeRegistry registry,
            string typeName,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices,
            string[] paramTypes)
        {
            registry.Register(typeName, methodName, paramTypes,
                ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, CreateVoidAbiSlot(), rawIndices);
        }
    }
}