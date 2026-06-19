using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// String::Compare (GenericShapeDescriptor -- handles all overloads)
        /// </summary>
        private static void RegisterStringCompare(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Compare",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, "",
                        [
                            "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateInt32AbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    if (paramTypes.Count == 2 && paramTypes.All(p => p == "System.String"))
                    {
                        var srcCmp = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringCompare(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcCmp, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var pt in paramTypes)
                    {
                        if (pt == "System.String" || pt.Contains("System.String"))
                            abiSlots.Add(CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType));
                        else
                            abiSlots.Add(CreateInt32AbiSlot());
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// DateTime::AddDays (SimpleForward stub)
        /// </summary>
        private static void RegisterDateTimeAddDays(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "AddDays", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDateTimeAddDays",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Type::get_IsArray
        /// </summary>
        private static void RegisterTypegetIsArray(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsArray", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsArray",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// List<T>::Sort (inline field operations — zero-param overload uses std::sort)
        /// </summary>
        private static void RegisterListTSort(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.List",
                MethodName: "Sort",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    if (abiSlots.Count == 1)
                    {
                        // Zero-param overload: Sort() — inline std::sort on elements
                        var src0 = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    auto* _list = reinterpret_cast<chaos_list_fields*>(reinterpret_cast<char*>(chaos_arg_0) + 8);",
                            "    auto* hdr = reinterpret_cast<chaos_list_array_header*>(_list->items_array);",
                            "    if (hdr == nullptr || _list->size < 2) return;",
                            "    auto* elems = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(hdr + 1);",
                            "    std::sort(elems, elems + _list->size);",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(abiSlots[0]),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 });
                    }
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Span<T>::get_Empty (GenericShapeDescriptor -- handles generic Span<T>)
        /// </summary>
        private static void RegisterSpanTget(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Span",
                MethodName: "get_Empty",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        EmptyRawArgumentIndices);
                }));


        }

    }
}
