using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// TimeSpan::FromDays (SimpleForward stub)
        /// </summary>
        private static void RegisterTimeSpanFromDays(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.TimeSpan", "FromDays", ["System.Double"],
                ShapeKind.SimpleForward, "ChaosTimeSpanFromDays",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// String::Split (GenericShapeDescriptor -- handles all overloads)
        /// </summary>
        private static void RegisterStringSplit(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "Split",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    // 2-param: Split(char) — delegate to ChaosStringSplit
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.Char")
                    {
                        var srcSplit = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringSplit(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcSplit, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateInt32AbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0, 1 });
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
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
        /// CompareInfo::IndexOf (SimpleForward stub)
        /// </summary>
        private static void RegisterCompareInfoIndexOf(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CompareInfo", "IndexOf", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosCompareInfoIndexOf",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Globalization.CompareInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1, 2 });

        }

        /// <summary>
        /// DateTime::DaysInMonth (SimpleForward stub)
        /// </summary>
        private static void RegisterDateTimeDaysInMonth(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "DaysInMonth", ["System.Int32", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDateTimeDaysInMonth",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// System.Type::get_IsNested (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemTypeget(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsNested", [],
                ShapeKind.SimpleForward, "ChaosTypeGetIsNested",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

    }
}
