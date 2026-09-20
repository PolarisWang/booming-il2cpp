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
            // ChaosXmlTextReaderCreate returns an opaque native handle into the
            // reader side table; the handle — not the freshly allocated managed
            // object address — is what every later `callvirt` must receive.
            // Without this flag the return value is dropped and Resolve() sees a
            // heap pointer where it expects a 1-based slot, silently returning
            // null (reader/writer subjects then record caught=True / realDefect).
            registry.Register("System.Xml.XmlTextReader", ".ctor",
                new[] { "System.IO.TextReader" },
                ShapeKind.SimpleForward, "ChaosXmlTextReaderCreate",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { trAbi }),
                CreateVoidAbiSlot(), new HashSet<int> { 0 },
                ctorReturnsNativeHandle: true);

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
            // ReturnAbi MUST be Void: the structured-EH linear newobj lowering
            // (EmitLinearNewObject) rejects a non-void constructor return with
            // NotSupportedException — "requires void constructor return ABI" —
            // which BuildMethodSourceSafe then swallows, replacing the whole
            // subject body with a stub.  The symptom is nasty because the shape
            // *resolves* fine and the native factory still returns the writer
            // handle; what breaks is that `new XmlTextWriter(...)` never reaches
            // the emitted body, so the following callvirt operates on whatever
            // else is on the eval stack — here the StringWriter instance — and
            // ChaosXmlWriter* then finds no WriterState for it and silently
            // returns without raising (110 XmlTextWriter subjects were affected:
            // realDefect + failed).  This is the same class as the earlier
            // ctor-ABI bug; `CreateVoidAbiSlot()` is what the other .ctor
            // registrations (e.g. System.Decimal) already use.
            registry.Register("System.Xml.XmlTextWriter", ".ctor",
                new[] { "System.IO.TextWriter" },
                ShapeKind.SimpleForward, "ChaosXmlTextWriterCreate",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { twAbi }),
                CreateVoidAbiSlot(), rawThis,
                ctorReturnsNativeHandle: true);

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

            // ── Abstract XmlWriter surface ──
            //
            // ATG subjects reach the writer two ways: `new XmlTextWriter(...)`
            // (concrete, registered above) and `XmlWriter.Create(...)` — whose
            // STATIC type is the abstract System.Xml.XmlWriter.  The latter is
            // what System_Xml_XmlWriterTests exercises, and without these shapes
            // every such call fell through to ChaosExternalRuntimeFallback and
            // the subject degenerated into an ATG smoke stub.
            //
            // The same native symbols serve both: ChaosXmlWriterCreate hands out
            // the opaque handle for either entry point, so an XmlWriter.Create
            // result is indistinguishable from an XmlTextWriter at the ABI level.
            // Method bodies are shared via RegisterXmlWriterVoid below, which
            // differs from the XmlTextWriter helper only in the receiver type key.
            RegisterXmlWriterStubs(registry);
        }

        /// <summary>
        /// Register the abstract System.Xml.XmlWriter surface, mirrored onto the
        /// same native symbols as XmlTextWriter.
        ///
        /// Why this is safe: the native side models the writer as an opaque
        /// handle into a process-local table, with no reference to the managed
        /// type hierarchy.  XmlTextWriter and XmlWriter.Create's returned
        /// XmlWellFormedWriter therefore share one implementation; registering
        /// both managed entry points against it only widens which subjects reach
        /// the code, it does not change the write semantics.
        /// </summary>
        private static void RegisterXmlWriterStubs(RuntimeHelperShapeRegistry registry)
        {
            var wAbi = CreateNativeIntAbiSlot(
                "System.Private.Xml/System.Xml.XmlWriter",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intRetAbi = CreateInt32AbiSlot();
            var rawThis = new HashSet<int> { 0 };

            var void0 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi });
            var ws = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi });
            var wsR = new HashSet<int> { 0, 1 };
            var ws2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, strAbi, strAbi });
            var ws2R = new HashSet<int> { 0, 1, 2 };
            var ws3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, strAbi, strAbi, strAbi });
            var ws3R = new HashSet<int> { 0, 1, 2, 3 };
            var arrAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Array",
                AotCoreIrTypeShapeKind.ReferenceType);
            var wc3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, arrAbi, intRetAbi, intRetAbi });
            var wc3R = new HashSet<int> { 0, 1, 2, 3 };
            var ce1 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, intRetAbi });
            var ce1R = new HashSet<int> { 0, 1 };
            var char2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, intRetAbi, intRetAbi });
            var char2R = new HashSet<int> { 0, 1, 2 };
            var dt4 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, strAbi, strAbi, strAbi, strAbi });
            var dt4R = new HashSet<int> { 0, 1, 2, 3, 4 };

            // 0-arg void
            RegisterXmlWriterVoid(registry, "Close",              "ChaosXmlWriterClose",              void0, rawThis, Array.Empty<string>());
            RegisterXmlWriterVoid(registry, "Flush",              "ChaosXmlWriterFlush",              void0, rawThis, Array.Empty<string>());
            RegisterXmlWriterVoid(registry, "WriteEndElement",    "ChaosXmlWriterWriteEndElement",    void0, rawThis, Array.Empty<string>());
            RegisterXmlWriterVoid(registry, "WriteFullEndElement","ChaosXmlWriterWriteFullEndElement",void0, rawThis, Array.Empty<string>());
            RegisterXmlWriterVoid(registry, "WriteEndDocument",   "ChaosXmlWriterWriteEndDocument",   void0, rawThis, Array.Empty<string>());
            RegisterXmlWriterVoid(registry, "WriteEndAttribute",  "ChaosXmlWriterWriteEndAttribute",  void0, rawThis, Array.Empty<string>());
            RegisterXmlWriterVoid(registry, "WriteStartDocument", "ChaosXmlWriterWriteStartDocument", void0, rawThis, Array.Empty<string>());

            // 1-arg string
            RegisterXmlWriterVoid(registry, "WriteString",     "ChaosXmlWriterWriteString",     ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteWhitespace", "ChaosXmlWriterWriteWhitespace", ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteRaw",        "ChaosXmlWriterWriteRaw",        ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteComment",    "ChaosXmlWriterWriteComment",    ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteCData",      "ChaosXmlWriterWriteCData",      ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteName",       "ChaosXmlWriterWriteName",       ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteNmToken",    "ChaosXmlWriterWriteNmToken",    ws, wsR, new[] { "System.String" });
            RegisterXmlWriterVoid(registry, "WriteEntityRef",  "ChaosXmlWriterWriteEntityRef",  ws, wsR, new[] { "System.String" });

            // 2-arg string
            RegisterXmlWriterVoid(registry, "WriteQualifiedName",        "ChaosXmlWriterWriteQualifiedName",        ws2, ws2R, new[] { "System.String", "System.String" });
            RegisterXmlWriterVoid(registry, "WriteProcessingInstruction","ChaosXmlWriterWriteProcessingInstruction",ws2, ws2R, new[] { "System.String", "System.String" });
            RegisterXmlWriterVoid(registry, "WriteAttributeString",      "ChaosXmlWriterWriteAttributeString",      ws2, ws2R, new[] { "System.String", "System.String" });

            // 3-arg string
            RegisterXmlWriterVoid(registry, "WriteStartElement",    "ChaosXmlWriterWriteStartElement3",    ws3, ws3R, new[] { "System.String", "System.String", "System.String" });
            RegisterXmlWriterVoid(registry, "WriteStartAttribute",  "ChaosXmlWriterWriteStartAttribute",   ws3, ws3R, new[] { "System.String", "System.String", "System.String" });
            RegisterXmlWriterVoid(registry, "WriteElementString",   "ChaosXmlWriterWriteElementString",    ws3, ws3R, new[] { "System.String", "System.String", "System.String" });

            // 4-arg string
            RegisterXmlWriterVoid(registry, "WriteDocType", "ChaosXmlWriterWriteDocType", dt4, dt4R, new[] { "System.String", "System.String", "System.String", "System.String" });

            // buffer/index/count
            RegisterXmlWriterVoid(registry, "WriteChars",  "ChaosXmlWriterWriteChars",  wc3, wc3R, new[] { "System.Char[]", "System.Int32", "System.Int32" });
            RegisterXmlWriterVoid(registry, "WriteBase64", "ChaosXmlWriterWriteBase64", wc3, wc3R, new[] { "System.Byte[]", "System.Int32", "System.Int32" });
            RegisterXmlWriterVoid(registry, "WriteBinHex", "ChaosXmlWriterWriteBinHex", wc3, wc3R, new[] { "System.Byte[]", "System.Int32", "System.Int32" });

            // char
            RegisterXmlWriterVoid(registry, "WriteCharEntity",          "ChaosXmlWriterWriteCharEntity",          ce1,   ce1R,   new[] { "System.Char" });
            RegisterXmlWriterVoid(registry, "WriteSurrogateCharEntity", "ChaosXmlWriterWriteSurrogateCharEntity", char2, char2R, new[] { "System.Char", "System.Char" });

            // WriteValue(object) — reflected value, written with invariant ToString.
            registry.Register("System.Xml.XmlWriter", "WriteValue",
                new[] { "System.Object" }, ShapeKind.SimpleForward,
                "ChaosXmlWriterWriteValue",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, arrAbi }),
                CreateVoidAbiSlot(), new HashSet<int> { 0, 1 });

            // LookupPrefix(string) -> string
            registry.Register("System.Xml.XmlWriter", "LookupPrefix",
                new[] { "System.String" }, ShapeKind.SimpleForward,
                "ChaosXmlWriterLookupPrefix", ws, strAbi,
                new HashSet<int> { 0, 1 });

            // ── WriteNode(XmlReader, bool) / WriteAttributes(XmlReader, bool) ──
            // Second argument is a bool carried in the int32 slot, consistent
            // with WriteStartDocument(bool) above.
            var node2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, arrAbi, intRetAbi });
            var node2R = new HashSet<int> { 0, 1, 2 };
            RegisterXmlWriterVoid(registry, "WriteNode",
                "ChaosXmlWriterWriteNode", node2, node2R,
                new[] { "System.Xml.XmlReader", "System.Boolean" });
            RegisterXmlWriterVoid(registry, "WriteAttributes",
                "ChaosXmlWriterWriteAttributes", node2, node2R,
                new[] { "System.Xml.XmlReader", "System.Boolean" });
        }

        /// <summary>
        /// Register one void-returning abstract-XmlWriter instance method as
        /// SimpleForward.  Mirrors <see cref="RegisterXmlTextWriterVoid"/> but
        /// keys on the abstract receiver type.
        /// </summary>
        private static void RegisterXmlWriterVoid(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices,
            string[] paramTypes)
        {
            registry.Register("System.Xml.XmlWriter", methodName,
                paramTypes,
                ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, CreateVoidAbiSlot(), rawIndices);
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
