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
            // NOTE: no second UInt64 registration here — WriteNumber(UInt64) is
            // registered once above (ChaosUtf8JsonWriterWriteNumberUInt).  A
            // duplicate shape key ("Shape already registered") throws in the
            // registry constructor and blocks EVERY chunk's codegen.

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
            registry.Register("Utf8JsonWriter", "Reset",
                new[] { "System.IO.Stream" }, ShapeKind.SimpleForward,
                "ChaosUtf8JsonWriterResetStream", wResetS,
                CreateVoidAbiSlot(), new HashSet<int> { 0, 1 });
            registry.Register("Utf8JsonWriter", "Reset",
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
                    new[] { "Utf8JsonWriter" }, ShapeKind.SimpleForward,
                    "ChaosUtf8JsonWriterWriteTo",
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                        new[] { recvAbi, wAbi }),
                    CreateVoidAbiSlot(), writeToR);
            }

            RegisterUtf8JsonWriterOverloadGaps(registry, wAbi, strAbi, objAbi, intAbi, voidAbi, rawThis);
        }

        /// <summary>
        /// B1: the overloads that had no native implementation at all.
        ///
        /// Every entry here needed a NEW native symbol — none could reuse an
        /// existing one, because the shape key is an exact (method, paramTypes)
        /// tuple and the C++ stub is selected by that key.  Two registers with
        /// the same (method, paramTypes) would also throw "Shape already
        /// registered" and kill codegen for the whole chunk.
        ///
        /// Exception semantics (measured on .NET 10 against a bare
        /// GetUninitializedObject instance):
        ///   * property-name writes  -> InvalidOperationException
        ///   * value-only writes     -> NullReferenceException
        ///   * WriteRawValue         -> NRE, except an empty payload ->
        ///                              InvalidOperationException
        /// The native stubs mirror exactly that; see json_writer_stubs.cpp.
        /// </summary>
        private static void RegisterUtf8JsonWriterOverloadGaps(
            RuntimeHelperShapeRegistry registry,
            AotCoreIrAbiSlotArtifact wAbi,
            AotCoreIrAbiSlotArtifact strAbi,
            AotCoreIrAbiSlotArtifact objAbi,
            AotCoreIrAbiSlotArtifact intAbi,
            AotCoreIrAbiSlotArtifact voidAbi,
            HashSet<int> rawThis)
        {
            // Parameter type display names, spelled the way the AOT core-IR does.
            const string TStr = "System.String";
            const string TEnc = "System.Text.Json.JsonEncodedText";
            const string TBool = "System.Boolean";
            const string TDateTime = "System.DateTime";
            const string TDateTimeOffset = "System.DateTimeOffset";
            const string TGuid = "System.Guid";
            const string TInt32 = "System.Int32";
            const string TUInt32 = "System.UInt32";
            const string TInt64 = "System.Int64";
            const string TUInt64 = "System.UInt64";
            const string TDecimal = "System.Decimal";
            const string TDouble = "System.Double";
            const string TSingle = "System.Single";

            var thisAndName = new HashSet<int> { 0, 1 };
            var thisNameValue = new HashSet<int> { 0, 1, 2 };

            void Reg(string method, string symbol, AotCoreIrAbiSlotArtifact[] slots,
                     HashSet<int> raw, params string[] paramTypes) =>
                registry.Register("Utf8JsonWriter", method, paramTypes,
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(slots),
                    voidAbi, raw);

            var wNameValue = new[] { wAbi, objAbi, objAbi };
            var wNameInt = new[] { wAbi, objAbi, intAbi };
            var wStrNameValue = new[] { wAbi, strAbi, objAbi };

            // ── WriteStartObject / WriteStartArray property-name forms ──
            Reg("WriteStartObject", "ChaosUtf8JsonWriterWriteStartObjectStr",
                new[] { wAbi, strAbi }, thisAndName, TStr);
            Reg("WriteStartObject", "ChaosUtf8JsonWriterWriteStartObjectEncoded",
                new[] { wAbi, objAbi }, thisAndName, TEnc);
            Reg("WriteStartArray", "ChaosUtf8JsonWriterWriteStartArrayStr",
                new[] { wAbi, strAbi }, thisAndName, TStr);
            Reg("WriteStartArray", "ChaosUtf8JsonWriterWriteStartArrayEncoded",
                new[] { wAbi, objAbi }, thisAndName, TEnc);

            // ── WriteString(propertyName, non-string value) ──
            // NOTE: (System.String, JsonEncodedText) is deliberately absent — it is
            // already registered above at the WriteString(string,string) block,
            // pointing at ChaosUtf8JsonWriterWriteStringEncodedText.  Registering it
            // again throws "Shape already registered" and kills ALL codegen for the
            // chunk (the whole pipeline then silently reuses a stale entry.exe).
            // string property name + struct value
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringStrDateTime",
                wStrNameValue, thisNameValue, TStr, TDateTime);
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringStrDateTimeOffset",
                wStrNameValue, thisNameValue, TStr, TDateTimeOffset);
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringStrGuid",
                wStrNameValue, thisNameValue, TStr, TGuid);
            // JsonEncodedText property name + struct/encoded/string value
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringEncodedDateTime",
                wNameValue, thisNameValue, TEnc, TDateTime);
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringEncodedDateTimeOffset",
                wNameValue, thisNameValue, TEnc, TDateTimeOffset);
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringEncodedGuid",
                wNameValue, thisNameValue, TEnc, TGuid);
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringEncodedEncoded",
                wNameValue, thisNameValue, TEnc, TEnc);
            Reg("WriteString", "ChaosUtf8JsonWriterWriteStringEncodedStr",
                wNameValue, thisNameValue, TEnc, TStr);

            // ── WriteNumber(propertyName, scalar) missing widths ──
            // NOTE: WriteNumber(System.String, System.UInt64) is already registered
            // elsewhere in this file — do NOT add it again ("Shape already
            // registered" aborts codegen for the entire chunk).
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberStrInt32",
                wNameInt, thisNameValue, TStr, TInt32);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberStrUInt32",
                wNameInt, thisNameValue, TStr, TUInt32);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberStrDecimal",
                wStrNameValue, thisNameValue, TStr, TDecimal);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedInt32",
                wNameInt, thisNameValue, TEnc, TInt32);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedInt64",
                new[] { wAbi, objAbi, objAbi }, thisNameValue, TEnc, TInt64);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedUInt32",
                wNameInt, thisNameValue, TEnc, TUInt32);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedUInt64",
                new[] { wAbi, objAbi, objAbi }, thisNameValue, TEnc, TUInt64);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedDouble",
                new[] { wAbi, objAbi, objAbi }, thisNameValue, TEnc, TDouble);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedFloat",
                new[] { wAbi, objAbi, objAbi }, thisNameValue, TEnc, TSingle);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedDecimal",
                wNameValue, thisNameValue, TEnc, TDecimal);
            Reg("WriteNumber", "ChaosUtf8JsonWriterWriteNumberEncodedStr",
                wNameValue, thisNameValue, TEnc, TStr);

            // ── WriteNull / WriteBoolean with JsonEncodedText name ──
            Reg("WriteNull", "ChaosUtf8JsonWriterWriteNullEncoded",
                new[] { wAbi, objAbi }, thisAndName, TEnc);
            Reg("WriteBoolean", "ChaosUtf8JsonWriterWriteBooleanEncoded",
                new[] { wAbi, objAbi, intAbi }, thisNameValue, TEnc, TBool);

            // ── WriteRawValue(payload, skipInputValidation) ──
            Reg("WriteRawValue", "ChaosUtf8JsonWriterWriteRawValueStrBool",
                new[] { wAbi, strAbi, intAbi }, thisNameValue, TStr, TBool);
            Reg("WriteRawValue", "ChaosUtf8JsonWriterWriteRawValueSequenceBool",
                new[] { wAbi, objAbi, intAbi }, thisNameValue,
                "System.Buffers.ReadOnlySequence<System.Byte>", TBool);

            // ── WriteNumberValue missing widths ──
            Reg("WriteNumberValue", "ChaosUtf8JsonWriterWriteNumberValueInt32",
                new[] { wAbi, intAbi }, thisAndName, TInt32);
            Reg("WriteNumberValue", "ChaosUtf8JsonWriterWriteNumberValueUInt32",
                new[] { wAbi, intAbi }, thisAndName, TUInt32);
        }

        private static void RegisterUtf8WriterVoid(
            RuntimeHelperShapeRegistry registry,
            string methodName,
            string nativeSymbol,
            _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact> abiSlots,
            HashSet<int> rawIndices,
            string[] paramTypes)
        {
            registry.Register("Utf8JsonWriter", methodName,
                paramTypes, ShapeKind.SimpleForward, nativeSymbol,
                abiSlots, CreateVoidAbiSlot(), rawIndices);
        }

        /// <summary>
        /// S21 Group B: JsonSerializer.SerializeToUtf8Bytes&lt;TValue&gt; scalar subset.
        ///
        /// The full JsonSerializer surface has no AOT body (no managed object-graph
        /// walk), so these subjects fall through to a 0-arg catch-all that returns
        /// null byte[] — and the one representative that actually ran then crashed
        /// the byte[] assertion (SEH-FAULT 0xe0000001).  The subjects only ever
        /// serialize primitive values, and for those the JSON text is fully
        /// determined by the value (invariant culture, default options), so a
        /// narrow native implementation is honest and complete:
        /// SerializeToUtf8Bytes&lt;int&gt;(0, null) -&gt; [48].
        ///
        /// Only the Int32 overload is implemented now.  Other TValue kinds keep the
        /// unverified/stub gap — they are NOT mis-serialized into a wrong value.
        /// </summary>
        private static void RegisterJsonSerializerUtf8BytesGroup(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "JsonSerializer",
                MethodName: "SerializeToUtf8Bytes",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (typeArgs is { Count: 1 } && typeArgs[0] == "System.Int32")
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            [
                                "    return ChaosJsonSerializerSerializeToUtf8BytesInt(chaos_arg_0, chaos_arg_1);",
                            ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot("System.Text.Json.JsonSerializerOptions", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    // Other TValue kinds keep the honest stub gap (return null via
                    // the fallback) rather than returning a fabricated value.
                    return null;
                }));
        }
    }
}