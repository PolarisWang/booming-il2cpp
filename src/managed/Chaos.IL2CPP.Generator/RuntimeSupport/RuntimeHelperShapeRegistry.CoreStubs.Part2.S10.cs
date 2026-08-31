using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// List<T>::Remove — InlineShapeDescriptor (no function call)
        /// </summary>
        private static void RegisterListTRemove(RuntimeHelperShapeRegistry registry)
        {
            // IILE lambda: linear scan + CHAOS_IL2CPP_MEMMOVE shift on inline field buffer.
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Remove",
                Resolver: static (callee, paramTypes) =>
                {
                    if (paramTypes.Count != 1) return null;
                    return "([&]() -> CHAOS_IL2CPP_INT32 { auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>({0}) + 8); auto* _elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(reinterpret_cast<char*>(_list->items_array) + sizeof(CHAOS_IL2CPP_INT32)); for (CHAOS_IL2CPP_INT32 _i = 0; _i < _list->size; _i++) { if (_elems[_i] == ({1})) { auto _shift = static_cast<CHAOS_IL2CPP_SIZE>(_list->size - _i - 1); if (_shift > 0) CHAOS_IL2CPP_MEMMOVE(&_elems[_i], &_elems[_i + 1], _shift * sizeof(CHAOS_IL2CPP_INTPTR)); _list->size--; _list->version++; return 1; } } return 0; })()";
                })
            { IsInstanceMethod = true });

        }

        /// <summary>
        /// List<T>::Clear (GenericShapeDescriptor for dispatch table)
        /// </summary>
        private static void RegisterListTClear_1(RuntimeHelperShapeRegistry registry)
        {
            // Kept as fallback for hotpatch dispatch / indirect call paths.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Clear",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    _list->size = 0;",
                        "    _list->version++;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// List<T>::Add (inline field operations)
        /// </summary>
        private static void RegisterListTAdd(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Add",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size >= hdr->capacity) {",
                        "        auto old_cap = (hdr != nullptr) ? hdr->capacity : 0;",
                        "        auto new_cap = (old_cap == 0) ? 4 : old_cap * 2;",
                        "        auto* new_buf = static_cast<CHAOS_IL2CPP_INTPTR*>(CHAOS_IL2CPP_MALLOC(sizeof(CHAOS_IL2CPP_INT32) + static_cast<CHAOS_IL2CPP_SIZE>(new_cap) * sizeof(CHAOS_IL2CPP_INTPTR)));",
                        "        auto* new_hdr = reinterpret_cast<chaos_list_array_header*>(new_buf);",
                        "        new_hdr->capacity = new_cap;",
                        "        auto* new_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(new_hdr + 1);",
                        "        if (hdr != nullptr && _list->size > 0) {",
                        "            auto* old_elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "            CHAOS_IL2CPP_MEMCPY(new_elems, old_elems, static_cast<CHAOS_IL2CPP_SIZE>(_list->size) * sizeof(CHAOS_IL2CPP_INTPTR));",
                        "            CHAOS_IL2CPP_FREE(hdr);",
                        "        }",
                        "        _list->items_array = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_hdr);",
                        "        hdr = new_hdr;",
                        "    }",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    elems[_list->size] = chaos_arg_1;",
                        "    _list->size++;",
                        "    _list->version++;",
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
        /// List<T>::Contains (inline field operations)
        /// </summary>
        private static void RegisterListTContains_1(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Contains",
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
                        "        if (elems[i] == chaos_arg_1) return 1;",
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
        /// List<T>::IndexOf (inline field operations)
        /// </summary>
        private static void RegisterListTIndexOf_1(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "IndexOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                        "    if (hdr == nullptr || _list->size == 0) return -1;",
                        "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                        "    for (CHAOS_IL2CPP_INT32 i = 0; i < _list->size; i++) {",
                        "        if (elems[i] == chaos_arg_1) return i;",
                        "    }",
                        "    return -1;",
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
        /// List<T>::AsReadOnly — returns the list itself cast to the read-only
        /// collection. ReadOnlyCollection wraps the same List; since the runtime
        /// represents both by the same backing array (at +8 field offset), returning
        /// the list pointer keeps iteration (string.Join, foreach) working.
        /// </summary>
        private static void RegisterListTAsReadOnly(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "AsReadOnly",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return chaos_arg_0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

        }

    }
}
