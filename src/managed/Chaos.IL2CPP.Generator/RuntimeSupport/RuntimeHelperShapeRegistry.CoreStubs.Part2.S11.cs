using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// List<T>::Remove (inline field operations)
        /// </summary>
        private static void RegisterListTRemove_1(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Remove",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size == 0) return 0;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    for (CHAOS_IL2CPP_INT32 i = 0; i < _list->size; i++) {",
                        "        if (elems[i] == chaos_arg_1) {",
                        "            auto move_count = _list->size - i - 1;",
                        "            if (move_count > 0) std::memmove(&elems[i], &elems[i + 1], static_cast<CHAOS_IL2CPP_SIZE>(move_count) * sizeof(CHAOS_IL2CPP_INTPTR));",
                        "            _list->size--;",
                        "            _list->version++;",
                        "            return 1;",
                        "        }",
                        "    }",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

        }

        /// <summary>
        /// List<T>::RemoveAt (inline field operations)
        /// </summary>
        private static void RegisterListTRemoveAt(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "RemoveAt",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || chaos_arg_1 < 0 || chaos_arg_1 >= _list->size) return;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    auto move_count = _list->size - chaos_arg_1 - 1;",
                        "    if (move_count > 0) std::memmove(&elems[chaos_arg_1], &elems[chaos_arg_1 + 1], static_cast<CHAOS_IL2CPP_SIZE>(move_count) * sizeof(CHAOS_IL2CPP_INTPTR));",
                        "    _list->size--;",
                        "    _list->version++;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));

        }

        /// <summary>
        /// Span<T>::CopyTo (GenericShapeDescriptor -- handles generic Span<T>)
        /// </summary>
        private static void RegisterSpanTCopyTo(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Span",
                MethodName: "CopyTo",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    (void)chaos_arg_0; (void)chaos_arg_1;",
                        "    CHAOS_IL2CPP_FAIL();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 });
                }));


        }

        /// <summary>
        /// Array::Clear (GenericShapeDescriptor for DirectNativeSymbol)
        /// </summary>
        private static void RegisterArrayClear(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Clear",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 3) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INT32 chaos_arg_2",
                    [
                        "    ChaosArrayClear(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateInt32AbiSlot(),
                            CreateInt32AbiSlot(),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "ChaosArrayClear_Inline",
                        DirectNativeHeader: "\"runtime_stubs/array_stubs.h\"");
                }));

        }

        /// <summary>
        /// Type::IsAssignableFrom
        /// </summary>
        private static void RegisterTypeIsAssignableFrom(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "IsAssignableFrom", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsAssignableFrom",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

    }
}
