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
        /// List&lt;T&gt;::AsReadOnly — returns a real ReadOnlyCollection&lt;T&gt; wrapper
        /// object that shares the source List's items_array (a malloc'd buffer, NOT
        /// a GC array reference — registered as pointer_count=0 in GcTypeLayout).
        ///
        /// The wrapper is a genuine reference type (see
        /// NativeAotLoweringPlanner._trackTypeRef + ObjectModelEmission struct
        /// emission): `struct chaos_type_...ReadOnlyCollection<T> : chaos_object`
        /// with a single `items_array` member at offset 8, so `is`/type-test and
        /// vtable dispatch behave like a real ReadOnlyCollection instead of a List.
        ///
        /// This replaces the previous loud `throw chaos_managed_exception{}` stub,
        /// which crashed HelloWorld at `String.Join(",", list.AsReadOnly())`.
        /// </summary>
        private static void RegisterListTAsReadOnly(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "AsReadOnly",
                Resolver: (planner, callee, typeArgs) =>
                {
                    // Derive the closed ReadOnlyCollection<T> subject id directly from the
                    // calling List<T>'s declaring-type subject id, replacing the
                    // "System.Collections.Generic.List" open-generic name with the
                    // "System.Collections.ObjectModel.ReadOnlyCollection" name. This
                    // preserves the assembly prefix (System.Private.CoreLib/ or
                    // its legacy alias) so the built subject id EXACTLY matches the raw
                    // reference-type subject id that _trackTypeRef / ObjectModelEmission /
                    // GcTypeLayout registered — guaranteeing the emitted
                    // chaos_type_*ReadOnlyCollection struct symbol resolves.
                    var declaringType = GetMethodDeclaringTypeSubjectId(callee);
                    const string listMarker = "System.Collections.Generic.List<";
                    var listMarkerIndex = declaringType.IndexOf(listMarker, StringComparison.Ordinal);
                    if (listMarkerIndex < 0)
                        return null;
                    var rocSubjectId = declaringType.Substring(0, listMarkerIndex) +
                        "System.Collections.ObjectModel.ReadOnlyCollection<" +
                        declaringType.Substring(listMarkerIndex + listMarker.Length);
                    // Normalize the assembly prefix so the emitted struct symbol
                    // (chaos_type_System_Private_CoreLib_*) matches the reference
                    // type registration in _trackTypeRef.
                    rocSubjectId = ManagedNaming.NormalizeSubjectIdAssembly(rocSubjectId);
                    var rocTypeSymbol = NativeAotLoweringPlanner.GetNativeTypeSymbol(rocSubjectId);
                    var rocTypeInfoSymbol = NativeAotLoweringPlanner.GetNativeTypeInfoSymbol(rocSubjectId);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    auto* chaos_list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                        $"    auto* chaos_roc = CHAOS_IL2CPP_NEW_GC({rocTypeSymbol}, {{}});",
                        $"    chaos_roc->header.type_info = {rocTypeInfoSymbol};",
                        // Share the source List's full backing state (items_array + size +
                        // version) as a read-only VIEW, mirroring
                        // System.Collections.ObjectModel ReadOnlyCollection<T> which wraps
                        // and exposes the underlying List's _items/_size. This keeps the
                        // ReadOnlyCollection traversable identically to its source List
                        // (String.Join iterates it via a chaos_list_fields view at offset +8).
                        "    chaos_roc->items_array = chaos_list->items_array;",
                        "    chaos_roc->size = chaos_list->size;",
                        "    chaos_roc->version = chaos_list->version;",
                        "    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_roc);",
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
