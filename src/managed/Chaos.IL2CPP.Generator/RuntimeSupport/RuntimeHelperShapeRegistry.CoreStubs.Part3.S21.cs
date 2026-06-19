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
                    // Int32 overload — delegate to ChaosBitConverterGetBytes
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.Int32")
                    {
                        var srcBytes = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    return ChaosBitConverterGetBytes(0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));",
                        ]);
                        return new GenericShapeResolution(srcBytes, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateInt32AbiSlot()),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
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
