using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.BitConverter::GetBytes (GenericShapeDescriptor -- stub returning null, real for Int32)
        /// </summary>
        private static void RegisterSystemBitConverterGetBytes(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.BitConverter",
                MethodName: "GetBytes",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        // Map the managed parameter to (carrier, C++ type, byte width).
                        // Int128/UInt128 have no carrier — they fall through to the
                        // return-0 fallback below.
                        var (carrier, ctype, width) = paramTypes[0] switch
                        {
                            "System.Boolean" => ("Int32", "CHAOS_IL2CPP_INT32", 1),
                            "System.Char" => ("Int32", "CHAOS_IL2CPP_INT32", 2),
                            "System.SByte" => ("Int32", "CHAOS_IL2CPP_INT32", 1),
                            "System.Int16" => ("Int32", "CHAOS_IL2CPP_INT32", 2),
                            "System.UInt16" => ("Int32", "CHAOS_IL2CPP_INT32", 2),
                            "System.Int32" => ("Int32", "CHAOS_IL2CPP_INT32", 4),
                            "System.UInt32" => ("Int32", "CHAOS_IL2CPP_INT32", 4),
                            "System.Int64" => ("Int64", "CHAOS_IL2CPP_INT64", 8),
                            "System.UInt64" => ("Int64", "CHAOS_IL2CPP_INT64", 8),
                            "System.Single" => ("Float32", "CHAOS_IL2CPP_FLOAT32", 4),
                            "System.Double" => ("Float64", "CHAOS_IL2CPP_FLOAT64", 8),
                            // Half rides the Float32 carrier (S25 precedent).  memcpy of
                            // the low 2 bytes is exact for 0.0 (probed default); other
                            // halves would need the Float16 bit pattern (accepted divergence).
                            "System.Half" => ("Float32", "CHAOS_IL2CPP_FLOAT32", 2),
                            _ => ("", "", 0),
                        };
                        if (carrier != "")
                        {
                            // Build the byte[] with the REAL allocator (ChaosArrayNew1D) so
                            // the result is layout-identical to the probe's expected array
                            // — header_data array MethodTable, element_type_shape=1,
                            // element_type_info = byte TypeInfoHot*.  The earlier stubs
                            // hand-rolled a ManagedArrayAccessor with header_data=0 /
                            // element_type_info=nullptr, which the byte[] assertion path
                            // rejects, and the Int32 entry even wrote length=1 for 4 bytes.
                            var cmacro = carrier == "Int32" ? "CHAOS_IL2CPP_INT32"
                                : carrier == "Int64" ? "CHAOS_IL2CPP_INT64"
                                : carrier == "Float32" ? "CHAOS_IL2CPP_FLOAT32"
                                : "CHAOS_IL2CPP_FLOAT64";
                            var srcBytes = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                                $"{cmacro} chaos_arg_0",
                            [
                                "    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(ChaosArrayNew1D(",
                                "        chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(),",
                                "        chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(),",
                                $"        1, {width}));",
                                "    if (arr == nullptr) return 0;",
                                $"    const {ctype} v = chaos_arg_0;",
                                $"    std::memcpy(accessor_get_elements(arr), &v, {width});",
                                "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);",
                            ]);
                            return new GenericShapeResolution(srcBytes, symbol,
                                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                    carrier == "Float32" ? new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float32, TypeShape = AotCoreIrTypeShapeKind.ValueType }
                                    : carrier == "Float64" ? new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }
                                    : carrier == "Int64" ? new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }
                                    : CreateInt32AbiSlot()),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                new HashSet<int> { 0 });
                        }
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>();
                    foreach (var pt in paramTypes)
                    {
                        if (pt.Contains("Int64") || pt.Contains("Double") || pt.Contains("Single"))
                            abiSlots.Add(new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType });
                        else
                            abiSlots.Add(CreateInt32AbiSlot());
                    }
                    if (abiSlots.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// System.BitConverter::ToInt32 (SimpleForward → ChaosBitConverterToInt32)
        /// </summary>
        private static void RegisterSystemBitConverterToInt32(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.BitConverter", "ToInt32", ["System.Byte[]", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitConverterToInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// System.BitConverter::ToDouble (SimpleForward → ChaosBitConverterToDouble)
        /// </summary>
        private static void RegisterSystemBitConverterToDouble(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.BitConverter", "ToDouble", ["System.Byte[]", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosBitConverterToDouble",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64 },
                new HashSet<int> { 0, 1 });


        }

        /// <summary>
        /// Marshal.GetFunctionPointerForDelegate<T>(T) — returns native function pointer via thunk registry
        /// </summary>
        private static void RegisterMarshalGetFunctionPointerForDelegateT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetFunctionPointerForDelegate",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 1) return null;
                    var delegateTypeId = typeArgs[0];
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* rs = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    auto* ts = chaos::il2cpp::runtime_core::GetCurrentThreadState();",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(",
                        "        chaos::il2cpp::runtime_core::MarshalGetFunctionPointerForDelegateImpl(",
                        "            rs, ts, chaos_arg_0, \"" + delegateTypeId + "\"));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Marshal.GetDelegateForFunctionPointer<T>(IntPtr) — delegates to runtime → native function dispatch thunk
        /// </summary>
        private static void RegisterMarshalGetDelegateForFunctionPointerT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetDelegateForFunctionPointer",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 1) return null;
                    var delegateTypeId = typeArgs[0];
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // Extract param_count from Func/Action generic type args at codegen time.
                    // This lets the native helper select the correct arity dispatch thunk
                    // without needing RegisterDelegateThunk to be called first.
                    int paramCount = ExtractDelegateArity(delegateTypeId);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* rs = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    auto* ts = chaos::il2cpp::runtime_core::GetCurrentThreadState();",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(",
                        "        chaos::il2cpp::runtime_core::MarshalGetDelegateForFunctionPointerImpl(",
                        "            rs, ts, chaos_arg_0, \"" + delegateTypeId + "\", " + paramCount + "));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

            // ── AnsiStringMarshaller stubs ──────────────────────────────────────
            // Source-generated marshalling for [DllImport] CharSet.Ansi.
            registry.Register("System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller",
                "ConvertToUnmanaged", ["System.String"],
                ShapeKind.SimpleForward, "ChaosAnsiStringMarshallerConvertToUnmanaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller",
                "ConvertToManaged", ["System.Byte*"],
                ShapeKind.SimpleForward, "ChaosAnsiStringMarshallerConvertToManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller",
                "Free", ["System.Byte*"],
                ShapeKind.SimpleForward, "ChaosAnsiStringMarshallerFree",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });
            // ── ComWrappers stubs (AOT: COM not available, return 0/null) ─────────
            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "RegisterForMarshalling", ["System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersRegisterForMarshalling",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "GetOrCreateComInterfaceForObject", ["System.Object", "System.Int32", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersGetOrCreateComInterfaceForObject",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "GetOrCreateObjectForComInstance", ["System.IntPtr", "System.Int32", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersGetOrCreateObjectForComInstance",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                CreateNativeIntAbiSlot(),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1, 2 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "TryGetComInstance", ["System.Object", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersTryGetComInstance",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.InteropServices.ComWrappers",
                "TryGetObject", ["System.IntPtr", "System.Runtime.InteropServices.ComWrappers"],
                ShapeKind.SimpleForward, "ChaosComWrappersTryGetObject",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                CreateNativeIntAbiSlot(),
                CreateNativeIntAbiSlot("System.Runtime.InteropServices.ComWrappers", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });

            // ── NativeLibrary stubs (DirectNativeSymbol) ──────────────────────────
            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "Load", ["System.String"],
                ShapeKind.SimpleForward, "ChaosNativeLibraryLoad",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "Free", ["System.IntPtr"],
                ShapeKind.SimpleForward, "ChaosNativeLibraryFree",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Int32", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "GetExport", ["System.IntPtr", "System.String"],
                ShapeKind.SimpleForward, "ChaosNativeLibraryGetExport",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                CreateNativeIntAbiSlot(),
                CreateNativeIntAbiSlot("System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Runtime.InteropServices.NativeLibrary",
                "GetMainProgramHandle", [],
                ShapeKind.SimpleForward, "ChaosNativeLibraryGetMainProgramHandle",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                CreateNativeIntAbiSlot(),
                new HashSet<int>());

        }

    }
}
