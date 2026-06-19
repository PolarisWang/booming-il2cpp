using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Marshal non-generic operations (inline body with runtime core calls)
        /// </summary>
        private static void RegisterMarshalnongeneric(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "AllocHGlobal",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    if (chaos_arg_0 < 0)",
                        "    {",
                        "        CHAOS_IL2CPP_FAIL();",
                        "    }",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalAllocHGlobal(",
                        "        chaos_runtime, static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateInt32AbiSlot()),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices);
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "FreeHGlobal",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    (void)chaos::il2cpp::runtime_core::MarshalFreeHGlobal(chaos_runtime, chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "FreeCoTaskMem",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    (void)chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(chaos_runtime, chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Marshal.StringToHGlobalAnsi(String) → IntPtr
        /// </summary>
        private static void RegisterMarshalStringToHGlobalAnsiIntPtr(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToHGlobalAnsi",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) return 0;",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalAnsi(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Marshal.StringToHGlobalUni(String) → IntPtr
        /// </summary>
        private static void RegisterMarshalStringToHGlobalUniIntPtr(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToHGlobalUni",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0) return 0;",
                        string.Empty,
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToHGlobalUni(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Marshal.PtrToStringAnsi(IntPtr, int) → String
        /// </summary>
        private static void RegisterMarshalPtrToStringAnsiString(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStringAnsi",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return chaos::il2cpp::runtime_core::MarshalPtrToStringAnsiIcall(",
                        "        chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateInt32AbiSlot(),
                        }),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0, 1 });
                }));

        }

        /// <summary>
        /// Marshal.UnsafeAddrOfPinnedArrayElement(Array, int) → IntPtr
        /// </summary>
        private static void RegisterMarshalUnsafeAddrOfPinnedArrayElementIntPtr(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "UnsafeAddrOfPinnedArrayElement",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    return chaos::il2cpp::runtime_core::MarshalUnsafeAddrOfPinnedArrayElement(",
                        "        reinterpret_cast<void*>(chaos_arg_0), chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StringToCoTaskMemUTF8",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    if (chaos_arg_0 == 0)",
                        "    {",
                        "        return 0;",
                        "    }",
                        string.Empty,
                        "    auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);",
                        "    (void)chaos_length; (void)chaos_utf8_data;",
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        "    return chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(",
                        "        chaos_runtime, nullptr, reinterpret_cast<void*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 });
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStringUTF8",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return chaos_arg_0 == 0",
                        "        ? 0",
                        "        : chaos_reflection_create_string_literal(reinterpret_cast<const char*>(chaos_arg_0));",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

        }

    }
}
