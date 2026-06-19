using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Object::ReferenceEquals
        /// </summary>
        private static void RegisterObjectReferenceEquals(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Object", "ReferenceEquals", ["System.Object", "System.Object"],
                ShapeKind.SimpleForward, "chaos_object_reference_equals",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Stream/IO (SimpleForward)
        /// </summary>
        private static void RegisterStreamIO(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.IO.Stream", "Flush", [],
                ShapeKind.SimpleForward, "ChaosStreamFlush",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.Stream", "Seek", ["System.Int64", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamSeek",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.IO.Stream", "SetLength", ["System.Int64"],
                ShapeKind.SimpleForward, "ChaosStreamSetLength",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.Stream", "get_Length", [],
                ShapeKind.SimpleForward, "ChaosStreamGetLength",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.Stream", "get_Position", [],
                ShapeKind.SimpleForward, "ChaosStreamGetPosition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.MemoryStream", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosMemoryStreamCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.StringReader", ".ctor", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringReaderCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.StringWriter", ".ctor", [],
                ShapeKind.SimpleForward, "ChaosStringWriterCtor",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextReader", "ReadLine", [],
                ShapeKind.SimpleForward, "ChaosTextReaderReadLine",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextReader", "ReadToEnd", [],
                ShapeKind.SimpleForward, "ChaosTextReaderReadToEnd",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextWriter", "Write", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTextWriterWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.TextWriter", "WriteLine", [],
                ShapeKind.SimpleForward, "ChaosTextWriterWriteLine",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.IO.TextWriter", "WriteLine", ["System.String"],
                ShapeKind.SimpleForward, "ChaosTextWriterWriteLineStr",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── Stream.Read/Write/CopyTo ──
            registry.Register("System.IO.Stream", "Read", ["System.Byte[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamRead",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.IO.Stream", "Write", ["System.Byte[]", "System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosStreamWrite",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1, 2, 3 });

            registry.Register("System.IO.Stream", "CopyTo", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosStreamCopyTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── MemoryStream(byte[]) ──
            registry.Register("System.IO.MemoryStream", ".ctor", ["System.Byte[]"],
                ShapeKind.SimpleForward, "ChaosMemoryStreamCtorWithBuffer",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ── BinaryReader ──
            registry.Register("System.IO.BinaryReader", ".ctor", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosBinaryReaderCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryReader", "ReadInt32", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            registry.Register("System.IO.BinaryReader", "ReadString", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadString",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.IO.BinaryReader", "ReadDouble", [],
                ShapeKind.SimpleForward, "ChaosBinaryReaderReadDouble",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

            // ── BinaryWriter ──
            registry.Register("System.IO.BinaryWriter", ".ctor", ["System.IO.Stream"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterCtor",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryWriter", "Write", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterWriteInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int32, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.IO.BinaryWriter", "Write", ["System.String"],
                ShapeKind.SimpleForward, "ChaosBinaryWriterWriteString",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // ═══════════════════════════════════════════════════════════════
            // System.Buffers — InlineShape
            // ═══════════════════════════════════════════════════════════════

            static string? GetReverseEndiannessExpression(string cppType)
            {
                return cppType switch
                {
                    "CHAOS_IL2CPP_INT8" or "CHAOS_IL2CPP_UINT8" => "{0}",
                    "CHAOS_IL2CPP_INT16" =>
                        "static_cast<CHAOS_IL2CPP_INT16>((static_cast<CHAOS_IL2CPP_UINT16>({0}) << 8) | (static_cast<CHAOS_IL2CPP_UINT16>({0}) >> 8))",
                    "CHAOS_IL2CPP_UINT16" =>
                        "({0} << 8) | ({0} >> 8)",
                    "CHAOS_IL2CPP_INT32" =>
                        "static_cast<CHAOS_IL2CPP_INT32>((static_cast<CHAOS_IL2CPP_UINT32>({0}) << 24) | ((static_cast<CHAOS_IL2CPP_UINT32>({0}) & 0x0000FF00) << 8) | ((static_cast<CHAOS_IL2CPP_UINT32>({0}) >> 8) & 0x0000FF00) | (static_cast<CHAOS_IL2CPP_UINT32>({0}) >> 24))",
                    "CHAOS_IL2CPP_UINT32" =>
                        "({0} << 24) | (({0} & 0x0000FF00) << 8) | (({0} >> 8) & 0x0000FF00) | ({0} >> 24)",
                    "CHAOS_IL2CPP_INT64" =>
                        "static_cast<CHAOS_IL2CPP_INT64>([&]() -> CHAOS_IL2CPP_UINT64 { CHAOS_IL2CPP_UINT64 _v = static_cast<CHAOS_IL2CPP_UINT64>({0}); _v = (_v & 0x00000000FFFFFFFF) << 32 | (_v >> 32); _v = (_v & 0x0000FFFF0000FFFF) << 16 | ((_v >> 16) & 0x0000FFFF0000FFFF); _v = (_v & 0x00FF00FF00FF00FF) << 8 | ((_v >> 8) & 0x00FF00FF00FF00FF); return _v; }())",
                    "CHAOS_IL2CPP_UINT64" =>
                        "[&]() -> CHAOS_IL2CPP_UINT64 { CHAOS_IL2CPP_UINT64 _v = {0}; _v = (_v & 0x00000000FFFFFFFF) << 32 | (_v >> 32); _v = (_v & 0x0000FFFF0000FFFF) << 16 | ((_v >> 16) & 0x0000FFFF0000FFFF); _v = (_v & 0x00FF00FF00FF00FF) << 8 | ((_v >> 8) & 0x00FF00FF00FF00FF); return _v; }()",
                    _ => null,
                };
            }

            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Buffers.Binary.BinaryPrimitives",
                MethodName: "ReverseEndianness",
                Resolver: (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    var cppType = MapTypeArgToCppType(paramTypes[0]);
                    if (cppType == null) return null;
                    return GetReverseEndiannessExpression(cppType);
                }));

            // Base64.GetMaxEncodedToUtf8Length(int) → ((length + 2) / 3) * 4
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Buffers.Text.Base64",
                MethodName: "GetMaxEncodedToUtf8Length",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32") return null;
                    return "(({0} + 2) / 3) * 4";
                }));

            // Base64.GetMaxDecodedFromUtf8Length(int) → (length / 4) * 3
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Buffers.Text.Base64",
                MethodName: "GetMaxDecodedFromUtf8Length",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32") return null;
                    return "({0} / 4) * 3";
                }));

        }

        /// <summary>
        /// Array.Empty<T> (GenericShapeDescriptor -- resolves to ChaosArrayEmpty stub)
        /// </summary>
        private static void RegisterArrayEmptyT(RuntimeHelperShapeRegistry registry)
        {
            // NOTE: Must be GenericShapeDescriptor, not SimpleForward, because the codegen
            // includes generic type args in the method name (e.g. "Empty<System.Byte>"),
            // which causes exact hash lookup to fail against the non-generic registration.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Empty",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var body = new[] { "    return ChaosArrayEmpty();" };
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "", body);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosArrayEmpty_Inline");
                }));

        }

        /// <summary>
        /// Type::GetMethod generic handler (GenericShapeDescriptor -- handles various overloads)
        /// </summary>
        private static void RegisterTypeGetMethodgeneric(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetMethod",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    (void)chaos_arg_0; (void)chaos_arg_1;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    var src2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    (void)chaos_arg_0; (void)chaos_arg_1; (void)chaos_arg_2;",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src2, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1, 2 });
                }));


        }

        /// <summary>
        /// ThrowHelper (dead-code safety stubs — should never be called in well-formed tests)
        /// </summary>
        private static void RegisterThrowHelper(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.ThrowHelper",
                MethodName: "ThrowArgumentNullException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        ["    CHAOS_IL2CPP_FAIL();", "    return 0;"]);
                    return new GenericShapeResolution(src, symbol,
                        new AotCoreIrAbiSlotArtifact[] { CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType) },
                        CreateVoidAbiSlot(), EmptyRawArgumentIndices);
                }));

        }

    }
}
