using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Marshal.DestroyStructure — free native resources for marshalled struct
        /// </summary>
        private static void RegisterMarshalDestroyStructurefree(RuntimeHelperShapeRegistry registry)
        {
            // Generic overload: DestroyStructure<T>(IntPtr) — uses static descriptor.
            // Non-generic overload: DestroyStructure(IntPtr, Type) — V1: no-op (runtime reflection path).
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "DestroyStructure",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 1)
                    {
                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                            "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                            $"    const auto* chaos_desc = {structDescSymbol};",
                            "    if (chaos_desc != nullptr) {",
                            "        chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(",
                            "            chaos_desc, reinterpret_cast<unsigned char*>(chaos_arg_0), chaos_runtime);",
                            "    }",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    // Non-generic overload: Marshal.DestroyStructure(IntPtr, Type)
                    // Extracts TypeInfoHot* from the managed Type object via runtime helper.
                    var nonGenericSymbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var nonGenericSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", nonGenericSymbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ::chaos::il2cpp::runtime_core::ChaosDestroyStructureByType(",
                        "        chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(nonGenericSrc, nonGenericSymbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }), CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));


        }

        /// <summary>
        /// GCHandle stubs (DirectNativeSymbol)
        /// </summary>
        private static void RegisterGCHandlestubs(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.GCHandle",
                MethodName: "Alloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosGCHandleAlloc(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGCHandleAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.GCHandle",
                MethodName: "Free",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosGCHandleFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGCHandleFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.GCHandle",
                MethodName: "FromIntPtr",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosGCHandleGetTarget(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosGCHandleGetTarget");
                }));

        }

        /// <summary>
        /// Marshal HR stubs (DirectNativeSymbol)
        /// </summary>
        private static void RegisterMarshalHRstubs(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetExceptionForHR",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    return ChaosMarshalGetExceptionForHR(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateInt32AbiSlot()),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosMarshalGetExceptionForHR");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "ThrowExceptionForHR",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    ChaosMarshalThrowExceptionForHR(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateInt32AbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosMarshalThrowExceptionForHR");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "GetHRForException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosMarshalGetHRForException(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosMarshalGetHRForException");
                }));


        }

        /// <summary>
        /// NativeMemory stubs (DirectNativeSymbol)
        /// </summary>
        private static void RegisterNativeMemorystubs(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Alloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosNativeMemoryAlloc(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Free",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosNativeMemoryFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "Realloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosNativeMemoryRealloc(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeMemoryRealloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedAlloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosNativeMemoryAlignedAlloc(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedAlloc");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedFree",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    ChaosNativeMemoryAlignedFree(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedFree");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.NativeMemory",
                MethodName: "AlignedRealloc",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    return ChaosNativeMemoryAlignedRealloc(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosNativeMemoryAlignedRealloc");
                }));

        }

        /// <summary>
        /// SafeBuffer stubs (DirectNativeSymbol)
        /// </summary>
        private static void RegisterSafeBufferstubs(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "get_ByteLength",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosSafeBufferGetByteLength(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosSafeBufferGetByteLength");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "Read",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_UINT8", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosSafeBufferReadByte(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot() }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "ChaosSafeBufferReadByte");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.SafeBuffer",
                MethodName: "Write",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_UINT8 chaos_arg_2",
                    [
                        "    ChaosSafeBufferWriteByte(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3] { CreateNativeIntAbiSlot(), CreateNativeIntAbiSlot(), CreateInt32AbiSlot() }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosSafeBufferWriteByte");
                }));

        }

    }
}
