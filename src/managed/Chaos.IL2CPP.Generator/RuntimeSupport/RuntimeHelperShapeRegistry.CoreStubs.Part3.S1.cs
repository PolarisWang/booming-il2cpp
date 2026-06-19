using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// DateTime::AddHours (SimpleForward stub)
        /// </summary>
        private static void RegisterDateTimeAddHours(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "AddHours", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosDateTimeAddHours",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType, TypeSubjectId = "System.Private.CoreLib/System.DateTime" },
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Type::get_BaseType
        /// </summary>
        private static void RegisterTypegetBaseType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_BaseType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetBaseType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Array::Sort (GenericShapeDescriptor -- calls ChaosArraySort for 1-param overload)
        /// </summary>
        private static void RegisterArraySort(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Sort",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var srcSort = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    ChaosArraySort(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(srcSort, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosArraySort_Inline");
                    }
                    if (paramTypes.Count == 2)
                    {
                        var srcSortCmp = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    ChaosArraySortWithComparer(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcSortCmp, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosArraySortWithComparer_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcSortFb = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(srcSortFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Array::Reverse (GenericShapeDescriptor -- calls ChaosArrayReverse for 1-param overload)
        /// </summary>
        private static void RegisterArrayReverse(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "Reverse",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 1)
                    {
                        var srcRev = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    ChaosArrayReverse(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(srcRev, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosArrayReverse_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcRevFb = RenderSimpleExternalRuntimeHelper("void", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                    ]);
                    return new GenericShapeResolution(srcRevFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateVoidAbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Type::IsAssignableTo
        /// </summary>
        private static void RegisterTypeIsAssignableTo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "IsAssignableTo", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsAssignableTo",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

    }
}
