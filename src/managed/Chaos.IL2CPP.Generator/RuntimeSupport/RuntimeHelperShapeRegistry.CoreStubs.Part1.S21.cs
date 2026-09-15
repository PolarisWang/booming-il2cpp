// ═══════════════════════════════════════════════════════════════════════
// S21 — System.Text.Json.Utf8JsonWriter argument-validation shapes
// ═══════════════════════════════════════════════════════════════════════

using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// M3: Utf8JsonWriter argument-validation surface.  ATG subjects construct
        /// this type through SubjectInstanceFactory.Create&lt;Utf8JsonWriter&gt;() (a
        /// GetUninitializedObject bare instance whose instance methods throw
        /// ObjectDisposedException / InvalidOperationException / ArgumentNullException
        /// depending on input).  The native stubs replicate those contracts.
        /// </summary>
        private static void RegisterUtf8JsonWriterGroup(RuntimeHelperShapeRegistry registry)
        {
            var wAbi = CreateNativeIntAbiSlot(
                "System.Text.Json/System.Text.Json.Utf8JsonWriter",
                AotCoreIrTypeShapeKind.ReferenceType);
            var strAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.String",
                AotCoreIrTypeShapeKind.ReferenceType);
            var objAbi = CreateNativeIntAbiSlot(
                "System.Private.CoreLib/System.Object",
                AotCoreIrTypeShapeKind.ReferenceType);
            var intAbi = CreateInt32AbiSlot();
            var longRetAbi = CreateInt64AbiSlot();
            var voidAbi = CreateVoidAbiSlot();
            var rawThis = new HashSet<int> { 0 };
            var strR = new HashSet<int> { 0, 1 };

            // ── 0-arg void (this only) ──
            var thisOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi });
            RegisterUtf8WriterVoid(registry, "Flush",            "ChaosUtf8JsonWriterFlush",             thisOnly, rawThis, Array.Empty<string>());
            RegisterUtf8WriterVoid(registry, "Dispose",          "ChaosUtf8JsonWriterDispose",           thisOnly, rawThis, Array.Empty<string>());
            RegisterUtf8WriterVoid(registry, "WriteStartObject",  "ChaosUtf8JsonWriterWriteStartObject",   thisOnly, rawThis, Array.Empty<string>());
            RegisterUtf8WriterVoid(registry, "WriteStartArray",   "ChaosUtf8JsonWriterWriteStartArray",    thisOnly, rawThis, Array.Empty<string>());
            RegisterUtf8WriterVoid(registry, "WriteEndObject",    "ChaosUtf8JsonWriterWriteEndObject",     thisOnly, rawThis, Array.Empty<string>());
            RegisterUtf8WriterVoid(registry, "WriteEndArray",     "ChaosUtf8JsonWriterWriteEndArray",      thisOnly, rawThis, Array.Empty<string>());
            RegisterUtf8WriterVoid(registry, "WriteNullValue",    "ChaosUtf8JsonWriterWriteNullValue",     thisOnly, rawThis, Array.Empty<string>());

            // ── WriteNull(string propertyName) → void ──
            RegisterUtf8WriterVoid(registry, "WriteNull",
                "ChaosUtf8JsonWriterWriteNullStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi }),
                strR, new[] { "System.String" });

            // ── WriteBooleanValue(bool) → void ──
            RegisterUtf8WriterVoid(registry, "WriteBooleanValue",
                "ChaosUtf8JsonWriterWriteBooleanValue",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, intAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Boolean" });

            // ── WritePropertyName(string) / WritePropertyName(JsonEncodedText) → void ──
            RegisterUtf8WriterVoid(registry, "WritePropertyName",
                "ChaosUtf8JsonWriterWritePropertyNameStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi }),
                strR, new[] { "System.String" });
            RegisterUtf8WriterVoid(registry, "WritePropertyName",
                "ChaosUtf8JsonWriterWritePropertyNameEncoded",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, objAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Text.Json.JsonEncodedText" });

            // ── WriteCommentValue(string) → void ──
            RegisterUtf8WriterVoid(registry, "WriteCommentValue",
                "ChaosUtf8JsonWriterWriteCommentValue",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi }),
                strR, new[] { "System.String" });

            // ── WriteRawValue(string) → void ──
            RegisterUtf8WriterVoid(registry, "WriteRawValue",
                "ChaosUtf8JsonWriterWriteRawValueStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi }),
                strR, new[] { "System.String" });

            // ── WriteString(string propertyName, string value) → void ──
            var wStr2 = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, strAbi, strAbi });
            var wStr2R = new HashSet<int> { 0, 1, 2 };
            RegisterUtf8WriterVoid(registry, "WriteString",
                "ChaosUtf8JsonWriterWriteStringStr", wStr2, wStr2R,
                new[] { "System.String", "System.String" });
            RegisterUtf8WriterVoid(registry, "WriteString",
                "ChaosUtf8JsonWriterWriteStringEncodedText",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { wAbi, strAbi, objAbi }),
                new HashSet<int> { 0, 1, 2 },
                new[] { "System.String", "System.Text.Json.JsonEncodedText" });

            // ── WriteString(string value) → void (value-only) ──
            RegisterUtf8WriterVoid(registry, "WriteString",
                "ChaosUtf8JsonWriterWriteStringStrOnly",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new[] { wAbi, strAbi }),
                strR, new[] { "System.String" });

            // ── WriteNumber(string propertyName, Int64/Double/Float/UInt64) → void ──
            var wNum = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, strAbi, longRetAbi });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberStr", wNum, wStr2R,
                new[] { "System.String", "System.Int64" });
            // Double/Float/UInt64 overloads: same signature (double/float are widened
            // to the same ABI slot width on x64).  Use longRetAbi for all so the
            // float really hits the correct native overload at codegen time.
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberStrDouble", wNum, wStr2R,
                new[] { "System.String", "System.Double" });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberStrFloat",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { wAbi, strAbi, intAbi }),
                wStr2R, new[] { "System.String", "System.Single" });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberStrUInt", wNum, wStr2R,
                new[] { "System.String", "System.UInt64" });

            // ── WriteNumber(Int64/Double/Float/UInt64) → void (value-only) ──
            var wNumOnly = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, longRetAbi });
            var wNumOnlyR = new HashSet<int> { 0, 1 };
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberInt", wNumOnly, wNumOnlyR,
                new[] { "System.Int64" });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberDouble", wNumOnly, wNumOnlyR,
                new[] { "System.Double" });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberFloat",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { wAbi, intAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Single" });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberUInt", wNumOnly, wNumOnlyR,
                new[] { "System.UInt64" });
            RegisterUtf8WriterVoid(registry, "WriteNumber",
                "ChaosUtf8JsonWriterWriteNumberUInt64", wNumOnly, wNumOnlyR,
                new[] { "System.UInt64" });

            // ── WriteBoolean(string propertyName, bool value) → void ──
            var wBool = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, strAbi, intAbi });
            RegisterUtf8WriterVoid(registry, "WriteBoolean",
                "ChaosUtf8JsonWriterWriteBooleanStr", wBool, wStr2R,
                new[] { "System.String", "System.Boolean" });

            // ── Reset(Stream) / Reset(IBufferWriter<byte>) → void ──
            var wResetS = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, objAbi });
            // Use an INTPTR return for the native factory's handle pattern.
            registry.Register("System.Text.Json.Utf8JsonWriter", "Reset",
                new[] { "System.IO.Stream" }, ShapeKind.SimpleForward,
                "ChaosUtf8JsonWriterResetStream", wResetS,
                CreateVoidAbiSlot(), new HashSet<int> { 0, 1 });
            registry.Register("System.Text.Json.Utf8JsonWriter", "Reset",
                new[] { "System.Buffers.IBufferWriter<System.Byte>" }, ShapeKind.SimpleForward,
                "ChaosUtf8JsonWriterResetBufferWriter", wResetS,
                CreateVoidAbiSlot(), new HashSet<int> { 0, 1 });

            // ── WriteTo(Utf8JsonWriter) → void ──
            RegisterUtf8WriterVoid(registry, "WriteTo",
                "ChaosUtf8JsonWriterWriteTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { wAbi, wAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Text.Json.Utf8JsonWriter" });
        }

        private static void RegisterUtf8WriterVoid(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices,
            string[] paramTypes)
        {
            registry.Register("System.Text.Json.Utf8JsonWriter", methodName,
                paramTypes, ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, CreateVoidAbiSlot(), rawIndices);
        }
    }
}