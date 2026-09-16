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
            // NOTE: WriteNumber(UInt64) and WriteNumber(UInt64) at lines 139/141
            // both register "System.UInt64" — the second raises
            // "Shape already registered".  Remove the duplicate (UInt64 and
            // UInt64 are the same type; the extra entry was intentional for
            // UInt32 but got mistyped to UInt64).

            // ── WriteNumberValue (value-only overloads) → void ──
            // "Value" suffix methods have no property-name argument: valid only
            // inside an array or at the root.  Same ABI slots as WriteNumber's
            // value-only overloads.
            RegisterUtf8WriterVoid(registry, "WriteNumberValue",
                "ChaosUtf8JsonWriterWriteNumberValueInt", wNumOnly, wNumOnlyR,
                new[] { "System.Int64" });
            RegisterUtf8WriterVoid(registry, "WriteNumberValue",
                "ChaosUtf8JsonWriterWriteNumberValueDouble", wNumOnly, wNumOnlyR,
                new[] { "System.Double" });
            RegisterUtf8WriterVoid(registry, "WriteNumberValue",
                "ChaosUtf8JsonWriterWriteNumberValueFloat",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { wAbi, intAbi }),
                new HashSet<int> { 0, 1 }, new[] { "System.Single" });
            RegisterUtf8WriterVoid(registry, "WriteNumberValue",
                "ChaosUtf8JsonWriterWriteNumberValueUInt", wNumOnly, wNumOnlyR,
                new[] { "System.UInt64" });
            // Decimal is a 16-byte value type passed by pointer to the box.
            var wDec = new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new[] { wAbi, objAbi });
            var wDecR = new HashSet<int> { 0, 1 };
            RegisterUtf8WriterVoid(registry, "WriteNumberValue",
                "ChaosUtf8JsonWriterWriteNumberValueDecimal", wDec, wDecR,
                new[] { "System.Decimal" });

            // ── WriteStringValue (value-only overloads) → void ──
            RegisterUtf8WriterVoid(registry, "WriteStringValue",
                "ChaosUtf8JsonWriterWriteStringValueStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new[] { wAbi, strAbi }),
                strR, new[] { "System.String" });
            // DateTime / DateTimeOffset / Guid: struct values passed by pointer.
            RegisterUtf8WriterVoid(registry, "WriteStringValue",
                "ChaosUtf8JsonWriterWriteStringValueStruct", wDec, wDecR,
                new[] { "System.DateTime" });
            RegisterUtf8WriterVoid(registry, "WriteStringValue",
                "ChaosUtf8JsonWriterWriteStringValueStruct", wDec, wDecR,
                new[] { "System.DateTimeOffset" });
            RegisterUtf8WriterVoid(registry, "WriteStringValue",
                "ChaosUtf8JsonWriterWriteStringValueStruct", wDec, wDecR,
                new[] { "System.Guid" });
            // JsonEncodedText: encoded value, no managed string decode.
            RegisterUtf8WriterVoid(registry, "WriteStringValue",
                "ChaosUtf8JsonWriterWriteStringValueEncoded", wDec, wDecR,
                new[] { "System.Text.Json.JsonEncodedText" });

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
            //
            // The ATG WriteTo subjects belong to JsonDocument, JsonElement and
            // JsonProperty — NOT to Utf8JsonWriter itself.  Each forwards to the
            // same native stub: the shared behaviour is "a null target writer is
            // an ArgumentNullException", which is what the bare-object subjects
            // pass.  Receiver-typed slots are created per declaring type so the
            // shape lookup key matches the subject's static type.
            var jdocAbi = CreateNativeIntAbiSlot(
                "System.Text.Json/System.Text.Json.JsonDocument",
                AotCoreIrTypeShapeKind.ReferenceType);
            var jelemAbi = CreateNativeIntAbiSlot(
                "System.Text.Json/System.Text.Json.JsonElement",
                AotCoreIrTypeShapeKind.ReferenceType);
            var jpropAbi = CreateNativeIntAbiSlot(
                "System.Text.Json/System.Text.Json.JsonProperty",
                AotCoreIrTypeShapeKind.ReferenceType);
            var writeToR = new HashSet<int> { 0, 1 };
            foreach (var (typeName, recvAbi) in new[]
            {
                ("System.Text.Json.JsonDocument", jdocAbi),
                ("System.Text.Json.JsonElement", jelemAbi),
                ("System.Text.Json.JsonProperty", jpropAbi),
            })
            {
                registry.Register(typeName, "WriteTo",
                    new[] { "System.Text.Json.Utf8JsonWriter" }, ShapeKind.SimpleForward,
                    "ChaosUtf8JsonWriterWriteTo",
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                        new[] { recvAbi, wAbi }),
                    CreateVoidAbiSlot(), writeToR);
            }
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