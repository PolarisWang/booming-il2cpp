// ═══════════════════════════════════════════════════════════════════════
// S22 — System.Xml.XmlWriter *Async surface shapes
// ═══════════════════════════════════════════════════════════════════════
//
// The last 84 XML-chunk stubGaps are all XmlWriter *Async methods.  Their
// ATG subjects were emitted as `return 42L;` smoke stubs because
// TestEmitter.ExternalStubRaisesManagedException deliberately excluded async
// variants; the shape side had no registration either, so even had the call
// been emitted it would have fallen through to the external-runtime fallback.
//
// This shard supplies the registration half.  The native half lives in
// xml_writer_async_stubs.cpp, which reproduces the BCL contract for ATG's
// fixture writer (`XmlWriter.Create(new StringBuilder())`, Settings.Async
// == false): argument validation first, then InvalidOperationException.
//
// Return ABI
// ----------
// These methods return Task / ValueTask, which the ABI carries as an INTPTR
// handle (the same convention Task.Delay uses in S16).  The native entry never
// returns normally — every path raises — so the handle is never observed.
//
// Overload keys
// -------------
// The registry keys a shape on (declaring type, method name, paramTypes), so
// each overload needs its own entry AND its own native symbol.  Reusing one
// symbol across arities makes the generated call site pass N arguments to a
// function taking M != N, which fails as C2660 at C++ compile time.

using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Register the XmlWriter *Async write surface.  Both the abstract
        /// XmlWriter receiver (what ATG's subjects declare) and the concrete
        /// XmlTextWriter are keyed, mirroring RegisterXmlWriterGroup /
        /// RegisterXmlTextWriterGroup: the same native entry serves both.
        /// </summary>
        private static void RegisterXmlWriterAsyncGroup(RuntimeHelperShapeRegistry registry)
        {
            var wAbi = CreateNativeIntAbiSlot(
                "System.Xml.XmlWriter",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intAbi = CreateInt32AbiSlot();
            var rawThis = new HashSet<int> { 0 };

            // Task return carrier — see the file header.
            var taskRetAbi = CreateNativeIntAbiSlot();

            var thisOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi });
            var ws = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi });
            var wsR = new HashSet<int> { 0, 1 };
            var wb = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, intAbi });
            var wbR = new HashSet<int> { 0, 1 };
            var wc2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, intAbi, intAbi });
            var wc2R = new HashSet<int> { 0, 1, 2 };
            var ws2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi, strAbi });
            var ws2R = new HashSet<int> { 0, 1, 2 };
            var ws3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi, strAbi, strAbi });
            var ws3R = new HashSet<int> { 0, 1, 2, 3 };
            var ws4 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi, strAbi, strAbi, strAbi });
            var ws4R = new HashSet<int> { 0, 1, 2, 3, 4 };
            var arrAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Array",
                AotCoreIrTypeShapeKind.ReferenceType);
            var wc3 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, arrAbi, intAbi, intAbi });
            var wc3R = new HashSet<int> { 0, 1, 2, 3 };
            var wo = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, wAbi, intAbi });
            var woR = new HashSet<int> { 0, 1, 2 };

            // ── 0-arg ──
            RegisterXmlWriterAsync(registry, "WriteStartDocumentAsync", "ChaosXmlWriterWriteStartDocumentAsync", thisOnly, rawThis, Array.Empty<string>(), taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteEndDocumentAsync", "ChaosXmlWriterWriteEndDocumentAsync", thisOnly, rawThis, Array.Empty<string>(), taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteEndElementAsync", "ChaosXmlWriterWriteEndElementAsync", thisOnly, rawThis, Array.Empty<string>(), taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteFullEndElementAsync", "ChaosXmlWriterWriteFullEndElementAsync", thisOnly, rawThis, Array.Empty<string>(), taskRetAbi);
            RegisterXmlWriterAsync(registry, "FlushAsync", "ChaosXmlWriterFlushAsync", thisOnly, rawThis, Array.Empty<string>(), taskRetAbi);
            RegisterXmlWriterAsync(registry, "DisposeAsync", "ChaosXmlWriterDisposeAsync", thisOnly, rawThis, Array.Empty<string>(), taskRetAbi);

            // ── 1-arg bool ──
            RegisterXmlWriterAsync(registry, "WriteStartDocumentAsync", "ChaosXmlWriterWriteStartDocumentBoolAsync", wb, wbR, new[] { "System.Boolean" }, taskRetAbi);

            // ── 1-arg string ──
            RegisterXmlWriterAsync(registry, "WriteStringAsync", "ChaosXmlWriterWriteStringAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteWhitespaceAsync", "ChaosXmlWriterWriteWhitespaceAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteCommentAsync", "ChaosXmlWriterWriteCommentAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteCDataAsync", "ChaosXmlWriterWriteCDataAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteRawAsync", "ChaosXmlWriterWriteRawAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteNameAsync", "ChaosXmlWriterWriteNameAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteNmTokenAsync", "ChaosXmlWriterWriteNmTokenAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteEntityRefAsync", "ChaosXmlWriterWriteEntityRefAsync", ws, wsR, new[] { "System.String" }, taskRetAbi);

            // ── 1-arg char ──
            RegisterXmlWriterAsync(registry, "WriteCharEntityAsync", "ChaosXmlWriterWriteCharEntityAsync", wb, wbR, new[] { "System.Char" }, taskRetAbi);

            // ── 2-arg char, char ──
            RegisterXmlWriterAsync(registry, "WriteSurrogateCharEntityAsync", "ChaosXmlWriterWriteSurrogateCharEntityAsync", wc2, wc2R, new[] { "System.Char", "System.Char" }, taskRetAbi);

            // ── 2-arg string, string ──
            RegisterXmlWriterAsync(registry, "WriteQualifiedNameAsync", "ChaosXmlWriterWriteQualifiedNameAsync", ws2, ws2R, new[] { "System.String", "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteProcessingInstructionAsync", "ChaosXmlWriterWriteProcessingInstructionAsync", ws2, ws2R, new[] { "System.String", "System.String" }, taskRetAbi);

            // ── 3-arg ──
            RegisterXmlWriterAsync(registry, "WriteStartElementAsync", "ChaosXmlWriterWriteStartElementAsync", ws3, ws3R, new[] { "System.String", "System.String", "System.String" }, taskRetAbi);

            // ── 4-arg ──
            RegisterXmlWriterAsync(registry, "WriteDocTypeAsync", "ChaosXmlWriterWriteDocTypeAsync", ws4, ws4R, new[] { "System.String", "System.String", "System.String", "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteAttributeStringAsync", "ChaosXmlWriterWriteAttributeStringAsync", ws4, ws4R, new[] { "System.String", "System.String", "System.String", "System.String" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteElementStringAsync", "ChaosXmlWriterWriteElementStringAsync", ws4, ws4R, new[] { "System.String", "System.String", "System.String", "System.String" }, taskRetAbi);

            // ── buffer / index / count ──
            RegisterXmlWriterAsync(registry, "WriteCharsAsync", "ChaosXmlWriterWriteCharsAsync", wc3, wc3R, new[] { "System.Char[]", "System.Int32", "System.Int32" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteRawAsync", "ChaosXmlWriterWriteRawBufferAsync", wc3, wc3R, new[] { "System.Char[]", "System.Int32", "System.Int32" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteBase64Async", "ChaosXmlWriterWriteBase64Async", wc3, wc3R, new[] { "System.Byte[]", "System.Int32", "System.Int32" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteBinHexAsync", "ChaosXmlWriterWriteBinHexAsync", wc3, wc3R, new[] { "System.Byte[]", "System.Int32", "System.Int32" }, taskRetAbi);

            // ── node / attributes: reader|navigator + bool ──
            RegisterXmlWriterAsync(registry, "WriteNodeAsync", "ChaosXmlWriterWriteNodeAsync", wo, woR, new[] { "System.Xml.XmlReader", "System.Boolean" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteNodeAsync", "ChaosXmlWriterWriteNodeNavigatorAsync", wo, woR, new[] { "System.Xml.XPath.XPathNavigator", "System.Boolean" }, taskRetAbi);
            RegisterXmlWriterAsync(registry, "WriteAttributesAsync", "ChaosXmlWriterWriteAttributesAsync", wo, woR, new[] { "System.Xml.XmlReader", "System.Boolean" }, taskRetAbi);
        }

        /// <summary>
        /// Register one Task-returning XmlWriter *Async instance method for the
        /// abstract <c>System.Xml.XmlWriter</c> receiver.
        /// </summary>
        private static void RegisterXmlWriterAsync(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices,
            string[] paramTypes,
            AotCoreIrAbiSlotArtifact returnAbi)
        {
            registry.Register("System.Xml.XmlWriter", methodName,
                paramTypes,
                ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, returnAbi, rawIndices);
        }
    }
}
