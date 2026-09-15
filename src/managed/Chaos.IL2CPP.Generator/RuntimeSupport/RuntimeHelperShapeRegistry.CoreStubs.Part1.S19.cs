using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
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
                or "WriteComment" or "WriteCData")
                return new[] { "System.String" };
            return Array.Empty<string>();
        }
    }
}
