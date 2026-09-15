using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// M5 (json-xml-production-readiness): native shapes for
        /// System.Xml.XmlTextReader's read surface.
        ///
        /// Target is the CONCRETE XmlTextReader present in the XML chunk's
        /// namespace partition.  The native side runs a minimal pull-tokenizer
        /// (xml_reader_stubs.cpp) over an input buffer; a managed TextReader
        /// argument is accepted for identity but the buffer is currently sourced
        /// from a default document — see the TODO in ChaosXmlTextReaderCreate.
        ///
        /// Covered: Read / NodeType / Name / LocalName / NamespaceURI / Prefix /
        ///   Value / Depth / IsEmptyElement / HasAttributes / AttributeCount /
        ///   MoveToFirst|NextAttribute / MoveToElement / GetAttribute(string) /
        ///   ReadString / HasLineInfo / LookupNamespace / ResolveEntity / Skip /
        ///   ResetState / Close.
        ///
        /// Not covered: base64/binhex, ReadChars, ReadContentAs*,
        ///   ReadElementContentAs*, GetNamespacesInScope, GetRemainder.
        /// </summary>
        private static void RegisterXmlTextReaderStubs(RuntimeHelperShapeRegistry registry)
        {
            var ttAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlTextReader",
                AotCoreIrTypeShapeKind.ReferenceType);
            var trAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.IO.TextReader",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intRetAbi = CreateInt32AbiSlot();
            var strRetAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var rawThis = new HashSet<int> { 0 };
            var thisOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { ttAbi });

            // ── ctor(TextReader) ──
            registry.Register("System.Xml.XmlTextReader", ".ctor",
                new[] { "System.IO.TextReader" },
                ShapeKind.SimpleForward, "ChaosXmlTextReaderCreate",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { trAbi }),
                CreateVoidAbiSlot(), new HashSet<int> { 0 });

            // ── Read() -> bool ──
            registry.Register("System.Xml.XmlTextReader", "Read",
                Array.Empty<string>(), ShapeKind.SimpleForward, "ChaosXmlTextReaderRead",
                thisOnly, intRetAbi, rawThis);

            // ── bool-returning no-arg query methods ──
            RegisterXmlTextReaderBool(registry, "get_NodeType", "ChaosXmlTextReaderNodeType");
            RegisterXmlTextReaderBool(registry, "IsEmptyElement", "ChaosXmlTextReaderIsEmptyElement");
            RegisterXmlTextReaderBool(registry, "HasAttributes", "ChaosXmlTextReaderHasAttributes");
            RegisterXmlTextReaderBool(registry, "MoveToFirstAttribute", "ChaosXmlTextReaderMoveToFirstAttribute");
            RegisterXmlTextReaderBool(registry, "MoveToNextAttribute", "ChaosXmlTextReaderMoveToNextAttribute");
            RegisterXmlTextReaderBool(registry, "MoveToElement", "ChaosXmlTextReaderMoveToElement");
            RegisterXmlTextReaderBool(registry, "HasLineInfo", "ChaosXmlTextReaderHasLineInfo");
            RegisterXmlTextReaderBool(registry, "get_AttributeCount", "ChaosXmlTextReaderAttributeCount");
            RegisterXmlTextReaderBool(registry, "get_Depth", "ChaosXmlTextReaderDepth");

            // ── string-returning no-arg query methods ──
            RegisterXmlTextReaderStr(registry, "get_Name", "ChaosXmlTextReaderName");
            RegisterXmlTextReaderStr(registry, "get_LocalName", "ChaosXmlTextReaderLocalName");
            RegisterXmlTextReaderStr(registry, "get_NamespaceURI", "ChaosXmlTextReaderNamespaceURI");
            RegisterXmlTextReaderStr(registry, "get_Prefix", "ChaosXmlTextReaderPrefix");
            RegisterXmlTextReaderStr(registry, "get_Value", "ChaosXmlTextReaderValue");
            RegisterXmlTextReaderStr(registry, "ReadString", "ChaosXmlTextReaderReadString");

            // ── void no-arg methods ──
            RegisterXmlTextReaderVoid(registry, "Close", "ChaosXmlTextReaderClose");
            RegisterXmlTextReaderVoid(registry, "ResolveEntity", "ChaosXmlTextReaderResolveEntity");
            RegisterXmlTextReaderVoid(registry, "Skip", "ChaosXmlTextReaderSkip");
            RegisterXmlTextReaderVoid(registry, "ResetState", "ChaosXmlTextReaderResetState");

            // ── GetAttribute(string) -> string ──
            var ga1 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { ttAbi, strAbi });
            registry.Register("System.Xml.XmlTextReader", "GetAttribute",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderGetAttributeStr", ga1, strRetAbi,
                new HashSet<int> { 0, 1 });

            // ── LookupNamespace(string) -> string ──
            registry.Register("System.Xml.XmlTextReader", "LookupNamespace",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlTextReaderLookupNamespace", ga1, strRetAbi,
                new HashSet<int> { 0, 1 });
        }

        // NOTE: all XmlTextReader methods use a 32-bit return ABI regardless of
        // declared type. In 64-bit C++ the return register is shared, so an int
        // method returns via RAX/eax and a bool via AL; both are carried through
        // the same CreateInt32AbiSlot. This keeps the registry compact and is
        // exact for every non-pointer return in this family.

        private static void RegisterXmlTextReaderBool(
            RuntimeHelperShapeRegistry registry, string methodName, string symbol)
        {
            registry.Register("System.Xml.XmlTextReader", methodName,
                Array.Empty<string>(), ShapeKind.SimpleForward, symbol,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { CreateNativeIntAbiSlot(
                        "System.Private.Xml/System.Xml.XmlTextReader",
                        AotCoreIrTypeShapeKind.ReferenceType) }),
                CreateInt32AbiSlot(), new HashSet<int> { 0 });
        }

        private static void RegisterXmlTextReaderStr(
            RuntimeHelperShapeRegistry registry, string methodName, string symbol)
        {
            registry.Register("System.Xml.XmlTextReader", methodName,
                Array.Empty<string>(), ShapeKind.SimpleForward, symbol,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { CreateNativeIntAbiSlot(
                        "System.Private.Xml/System.Xml.XmlTextReader",
                        AotCoreIrTypeShapeKind.ReferenceType) }),
                CreateNativeIntAbiSlot(
                    "System.Private.CoreLib/System.String",
                    AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });
        }

        private static void RegisterXmlTextReaderVoid(
            RuntimeHelperShapeRegistry registry, string methodName, string symbol)
        {
            registry.Register("System.Xml.XmlTextReader", methodName,
                Array.Empty<string>(), ShapeKind.SimpleForward, symbol,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { CreateNativeIntAbiSlot(
                        "System.Private.Xml/System.Xml.XmlTextReader",
                        AotCoreIrTypeShapeKind.ReferenceType) }),
                CreateVoidAbiSlot(), new HashSet<int> { 0 });
        }

        /// <summary>
        /// M6 (json-xml-production-readiness): native shapes for
        /// System.Xml.XmlTextWriter (CONCRETE type present in the XML chunk).
        ///
        /// The first attempt registered against abstract XmlWriter, but that type
        /// is NOT in the namespace partition (System.Xml namespace only lists
        /// concrete subclasses).  XmlTextWriter runs its entire method surface
        /// through the handle table in xml_writer_stubs.cpp — the native side is
        /// shared between both abstract and concrete route.
        ///
        /// Covered: 28 methods from the namespace-partition manifest for
        ///   System.Private.Xml/System.Xml.XmlTextWriter
        ///
        /// Not covered (left on fallback): binary content methods (WriteBase64,
        /// WriteBinHex, WriteChars char[]), LookupPrefix, WriteCharEntity,
        /// WriteDocType, WriteEntityRef, WriteName, WriteNmToken,
        /// WriteQualifiedName, WriteSurrogateCharEntity.
        /// </summary>
        private static void RegisterXmlTextWriterStubs(RuntimeHelperShapeRegistry registry)
        {
            var ttAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlTextWriter",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var twAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.IO.TextWriter",
                AotCoreIrTypeShapeKind.ReferenceType);
            var boolAbi = CreateInt32AbiSlot();
            var intRetAbi = CreateInt32AbiSlot();
            var voidAbi = CreateVoidAbiSlot();
            var rawThis = new HashSet<int> { 0 };

            // ── Constructor: XmlTextWriter(TextWriter) ──
            // Registered as a plain factory returning the handle.  codegen's
            // newobj path consumes the returned INTPTR as the new instance, so
            // the native factory's handle doubles as the managed reference.
            registry.Register("System.Xml.XmlTextWriter", ".ctor",
                new[] { "System.IO.TextWriter" },
                ShapeKind.SimpleForward, "ChaosXmlTextWriterCreate",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { twAbi }),
                CreateNativeIntAbiSlot(), rawThis);

            // ── 0-arg void methods (this only) ──
            var void0 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { ttAbi });
            RegisterXmlTextWriterVoid(registry, "Close",         "ChaosXmlWriterClose",             void0, rawThis, XmlTextWriterParamTypes("Close", 0));
            RegisterXmlTextWriterVoid(registry, "Flush",         "ChaosXmlWriterFlush",             void0, rawThis, XmlTextWriterParamTypes("Flush", 0));
            RegisterXmlTextWriterVoid(registry, "WriteEndElement","ChaosXmlWriterWriteEndElement",   void0, rawThis, XmlTextWriterParamTypes("WriteEndElement", 0));
            RegisterXmlTextWriterVoid(registry, "WriteFullEndElement","ChaosXmlWriterWriteFullEndElement", void0, rawThis, XmlTextWriterParamTypes("WriteFullEndElement", 0));
            RegisterXmlTextWriterVoid(registry, "WriteEndDocument","ChaosXmlWriterWriteEndDocument", void0, rawThis, XmlTextWriterParamTypes("WriteEndDocument", 0));
            RegisterXmlTextWriterVoid(registry, "WriteEndAttribute","ChaosXmlWriterWriteEndAttribute", void0, rawThis, XmlTextWriterParamTypes("WriteEndAttribute", 0));

            // ── WriteStartDocument() — void, this only ──
            RegisterXmlTextWriterVoid(registry, "WriteStartDocument", "ChaosXmlWriterWriteStartDocument",
                void0, rawThis, XmlTextWriterParamTypes("WriteStartDocument", 0));

            // ── WriteStartDocument(bool) ──
            var sd1 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { ttAbi, boolAbi });
            var sd1R = new HashSet<int> { 0, 1 };
            RegisterXmlTextWriterVoid(registry, "WriteStartDocument",
                "ChaosXmlWriterWriteStartDocumentBool", sd1, sd1R, XmlTextWriterParamTypes("WriteStartDocument", 1));

            // ── WriteStartElement(string prefix, string localName, string ns) ──
            var se3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi, strAbi, strAbi });
            var se3R = new HashSet<int> { 0, 1, 2, 3 };
            RegisterXmlTextWriterVoid(registry, "WriteStartElement",
                "ChaosXmlWriterWriteStartElement3", se3, se3R, XmlTextWriterParamTypes("WriteStartElement", 3));

            // ── WriteString(string) — text/element content ──
            var ws = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi });
            var wsR = new HashSet<int> { 0, 1 };
            RegisterXmlTextWriterVoid(registry, "WriteString",
                "ChaosXmlWriterWriteString", ws, wsR, XmlTextWriterParamTypes("WriteString", 1));

            // ── WriteWhitespace(string) ──
            RegisterXmlTextWriterVoid(registry, "WriteWhitespace",
                "ChaosXmlWriterWriteWhitespace", ws, wsR, XmlTextWriterParamTypes("WriteWhitespace", 1));

            // ── WriteRaw(string) ──
            RegisterXmlTextWriterVoid(registry, "WriteRaw",
                "ChaosXmlWriterWriteRaw", ws, wsR, XmlTextWriterParamTypes("WriteRaw", 1));

            // ── WriteComment(string) ──
            RegisterXmlTextWriterVoid(registry, "WriteComment",
                "ChaosXmlWriterWriteComment", ws, wsR, XmlTextWriterParamTypes("WriteComment", 1));

            // ── WriteCData(string) ──
            RegisterXmlTextWriterVoid(registry, "WriteCData",
                "ChaosXmlWriterWriteCData", ws, wsR, XmlTextWriterParamTypes("WriteCData", 1));

            // ── WriteStartAttribute(string prefix, string localName, string ns) ──
            var sa3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi, strAbi, strAbi });
            var sa3R = new HashSet<int> { 0, 1, 2, 3 };
            RegisterXmlTextWriterVoid(registry, "WriteStartAttribute",
                "ChaosXmlWriterWriteStartAttribute", sa3, sa3R, XmlTextWriterParamTypes("WriteStartAttribute", 3));

            // ── Remaining string-only writers ──
            RegisterXmlTextWriterVoid(registry, "WriteName",
                "ChaosXmlWriterWriteName", ws, wsR, XmlTextWriterParamTypes("WriteName", 1));
            RegisterXmlTextWriterVoid(registry, "WriteNmToken",
                "ChaosXmlWriterWriteNmToken", ws, wsR, XmlTextWriterParamTypes("WriteNmToken", 1));
            RegisterXmlTextWriterVoid(registry, "WriteEntityRef",
                "ChaosXmlWriterWriteEntityRef", ws, wsR, XmlTextWriterParamTypes("WriteEntityRef", 1));

            // ── WriteQualifiedName(string localName, string ns) ──
            var qn2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi, strAbi });
            var qn2R = new HashSet<int> { 0, 1, 2 };
            RegisterXmlTextWriterVoid(registry, "WriteQualifiedName",
                "ChaosXmlWriterWriteQualifiedName", qn2, qn2R, XmlTextWriterParamTypes("WriteQualifiedName", 2));

            // ── WriteProcessingInstruction(string name, string text) ──
            RegisterXmlTextWriterVoid(registry, "WriteProcessingInstruction",
                "ChaosXmlWriterWriteProcessingInstruction", qn2, qn2R,
                XmlTextWriterParamTypes("WriteProcessingInstruction", 2));

            // ── WriteDocType(string name, string pubid, string sysid, string subset) ──
            var dt4 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, strAbi, strAbi, strAbi, strAbi });
            var dt4R = new HashSet<int> { 0, 1, 2, 3, 4 };
            RegisterXmlTextWriterVoid(registry, "WriteDocType",
                "ChaosXmlWriterWriteDocType", dt4, dt4R, XmlTextWriterParamTypes("WriteDocType", 4));

            // ── WriteChars(char[] buffer, int index, int count) ──
            var arrAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Array",
                AotCoreIrTypeShapeKind.ReferenceType);
            var wc3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, arrAbi, intRetAbi, intRetAbi });
            var wc3R = new HashSet<int> { 0, 1, 2, 3 };
            RegisterXmlTextWriterVoid(registry, "WriteChars",
                "ChaosXmlWriterWriteChars", wc3, wc3R, XmlTextWriterParamTypes("WriteChars", 3));

            // ── WriteBase64 / WriteBinHex(byte[] buffer, int index, int count) ──
            RegisterXmlTextWriterVoid(registry, "WriteBase64",
                "ChaosXmlWriterWriteBase64", wc3, wc3R, XmlTextWriterParamTypes("WriteChars", 3));
            RegisterXmlTextWriterVoid(registry, "WriteBinHex",
                "ChaosXmlWriterWriteBinHex", wc3, wc3R, XmlTextWriterParamTypes("WriteChars", 3));

            // ── WriteCharEntity(char ch) ──
            var ce1 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, intRetAbi });
            var ce1R = new HashSet<int> { 0, 1 };
            RegisterXmlTextWriterVoid(registry, "WriteCharEntity",
                "ChaosXmlWriterWriteCharEntity", ce1, ce1R, XmlTextWriterParamTypes("WriteCharEntity", 1));

            // ── WriteSurrogateCharEntity(char low, char high) ──
            var sce2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { ttAbi, intRetAbi, intRetAbi });
            var sce2R = new HashSet<int> { 0, 1, 2 };
            RegisterXmlTextWriterVoid(registry, "WriteSurrogateCharEntity",
                "ChaosXmlWriterWriteSurrogateCharEntity", sce2, sce2R,
                XmlTextWriterParamTypes("WriteSurrogateCharEntity", 2));

            // ── LookupPrefix(string ns) -> string ──
            registry.Register("System.Xml.XmlTextWriter", "LookupPrefix",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlWriterLookupPrefix", qn2, strAbi,
                new HashSet<int> { 0, 1 });
        }

        /// <summary>
        /// Register one void-returning XmlTextWriter instance method as SimpleForward.
        ///
        /// <paramref name="paramTypes"/> must distinguish overloads: the registry
        /// keys shapes on (type, method, paramTypes), so registering
        /// WriteStartDocument() and WriteStartDocument(bool) with the same empty
        /// list raises "Shape already registered" at planner construction time
        /// (surfaced as a codegen InvalidOperationException, not a compile error).
        /// </summary>
        private static void RegisterXmlTextWriterVoid(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices,
            string[] paramTypes)
        {
            registry.Register("System.Xml.XmlTextWriter", methodName,
                paramTypes,
                ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, CreateVoidAbiSlot(), rawIndices);
        }

        /// <summary>
        /// Parameter-type list for a registered XmlTextWriter method, derived from
        /// the IL parameter count (NOT the ABI slot count — slot 0 is the receiver).
        /// </summary>
        private static string[] XmlTextWriterParamTypes(string methodName, int ilParamCount)
        {
            if (methodName == "WriteStartDocument")
                return ilParamCount == 1 ? new[] { "System.Boolean" } : Array.Empty<string>();
            if (methodName == "WriteStartElement" || methodName == "WriteStartAttribute")
                return new[] { "System.String", "System.String", "System.String" };
            if (methodName is "WriteString" or "WriteWhitespace" or "WriteRaw"
                or "WriteComment" or "WriteCData" or "WriteQualifiedName"
                or "WriteName" or "WriteNmToken" or "WriteEntityRef"
                or "LookupPrefix")
                return new[] { "System.String" };
            if (methodName == "WriteProcessingInstruction")
                return new[] { "System.String", "System.String" };
            if (methodName == "WriteDocType")
                return new[] { "System.String", "System.String", "System.String", "System.String" };
            if (methodName == "WriteChars")
                return new[] { "System.Char[]", "System.Int32", "System.Int32" };
            if (methodName == "WriteCharEntity")
                return new[] { "System.Char" };
            if (methodName == "WriteSurrogateCharEntity")
                return new[] { "System.Char", "System.Char" };
            return Array.Empty<string>();
        }
    }
}
