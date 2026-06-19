using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Marshal.SizeOf<T> — return marshalled struct size via descriptor
        /// </summary>
        private static void RegisterMarshalSizeOfT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "SizeOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "", [
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    return chaos_desc != nullptr",
                        "        ? static_cast<CHAOS_IL2CPP_INT32>(chaos_desc->total_size)",
                        "        : 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>([]),
                        CreateInt32AbiSlot(),
                        new HashSet<int>());
                }));

        }

        /// <summary>
        /// Marshal.OffsetOf<T>(string) — return field offset by name via descriptor
        /// </summary>
        private static void RegisterMarshalOffsetOfT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "OffsetOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var fieldNamesSymbol = NativeAotLoweringPlanner.GetNativeStructFieldNamesSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ::chaos::il2cpp::runtime_core::MarshalOffsetOf(",
                        $"        {structDescSymbol},",
                        $"        {fieldNamesSymbol},",
                        "        chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Marshal.Copy (element type and direction extracted from parameter types)
        /// </summary>
        private static void RegisterMarshalCopy(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "Copy",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count != 0) return null;
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 4) return null;

                    bool isArrayToPtr;
                    string elementTypeName;
                    if (paramTypes[0].EndsWith("[]", StringComparison.Ordinal))
                    {
                        isArrayToPtr = true;
                        elementTypeName = paramTypes[0].Substring(0, paramTypes[0].Length - 2);
                    }
                    else if (string.Equals(paramTypes[0], "System.IntPtr", StringComparison.Ordinal) &&
                             paramTypes[1].EndsWith("[]", StringComparison.Ordinal))
                    {
                        isArrayToPtr = false;
                        elementTypeName = paramTypes[1].Substring(0, paramTypes[1].Length - 2);
                    }
                    else
                    {
                        return null;
                    }

                    if (!MarshalCopyElementTypeMap.TryGetValue(elementTypeName, out var cppElementType))
                        return null;

                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);

                    if (isArrayToPtr)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3",
                        [
                            "    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_0);",
                            "    auto chaos_start_index = chaos_arg_1;",
                            "    auto chaos_dest = chaos_arg_2;",
                            "    auto chaos_length = chaos_arg_3;",
                            "    chaos::il2cpp::runtime_core::MarshalCopyArrayToPtr<" + cppElementType + ">(chaos_array, chaos_start_index, chaos_dest, chaos_length);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(),
                                CreateInt32AbiSlot(),
                            }), CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                    else
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2, CHAOS_IL2CPP_INT32 chaos_arg_3",
                        [
                            "    auto chaos_source = chaos_arg_0;",
                            "    auto* chaos_array = reinterpret_cast<void*>(chaos_arg_1);",
                            "    auto chaos_start_index = chaos_arg_2;",
                            "    auto chaos_length = chaos_arg_3;",
                            "    chaos::il2cpp::runtime_core::MarshalCopyPtrToArray<" + cppElementType + ">(chaos_source, chaos_array, chaos_start_index, chaos_length);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[4]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                            }), CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2, 3 });
                    }
                }));

        }

        /// <summary>
        /// Marshal.StructureToPtr<T> — deep-copy managed struct to native memory
        /// </summary>
        private static void RegisterMarshalStructureToPtrT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "StructureToPtr",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    if (chaos_desc != nullptr) {",
                        "        if (chaos_arg_2 != 0) {",
                        "            chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(",
                        "                chaos_desc, reinterpret_cast<unsigned char*>(chaos_arg_1), chaos_runtime);",
                        "        }",
                        "        chaos::il2cpp::struct_marshal::MarshalStructManagedToNative(",
                        "            chaos_desc,",
                        "            reinterpret_cast<unsigned char*>(chaos_arg_1),",
                        "            reinterpret_cast<unsigned char*>(chaos_arg_0),",
                        "            chaos_runtime, nullptr);",
                        "    }",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(typeArgs[0], AotCoreIrTypeShapeKind.ValueType),
                            CreateNativeIntAbiSlot(),
                            CreateInt32AbiSlot(),
                        }), CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

        }

        /// <summary>
        /// Marshal.PtrToStructure<T> — deep-copy native memory to managed struct
        /// </summary>
        private static void RegisterMarshalPtrToStructureT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.Marshal",
                MethodName: "PtrToStructure",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs.Count == 0) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var structDescSymbol = NativeAotLoweringPlanner.GetNativeStructMarshallingDescriptorSymbol(typeArgs[0]);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_runtime = chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
                        "    if (chaos_runtime == nullptr) { CHAOS_IL2CPP_FAIL(); }",
                        $"    const auto* chaos_desc = {structDescSymbol};",
                        "    if (chaos_desc == nullptr) return 0;",
                        "    auto* chaos_blob = static_cast<unsigned char*>(",
                        "        CHAOS_IL2CPP_MALLOC(chaos_desc->total_size));",
                        "    if (chaos_blob == nullptr) return 0;",
                        "    chaos::il2cpp::struct_marshal::MarshalStructNativeToManaged(",
                        "        chaos_desc, chaos_blob,",
                        "        reinterpret_cast<const unsigned char*>(chaos_arg_0),",
                        "        chaos_runtime, nullptr);",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_blob);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(typeArgs[0], AotCoreIrTypeShapeKind.ValueType),
                        new HashSet<int> { 0 });
                }));

        }

    }
}
