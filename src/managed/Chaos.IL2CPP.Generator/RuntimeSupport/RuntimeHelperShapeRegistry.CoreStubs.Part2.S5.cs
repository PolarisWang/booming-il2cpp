using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Type::get_FullName
        /// </summary>
        private static void RegisterTypegetFullName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_FullName", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFullName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_IsValueType
        /// </summary>
        private static void RegisterTypegetIsValueType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsValueType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsValueType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// RuntimeHelpers::GetHashCode
        /// </summary>
        private static void RegisterRuntimeHelpersGetHashCode(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Runtime.CompilerServices.RuntimeHelpers", "GetHashCode", ["System.Object"],
                ShapeKind.SimpleForward, "ChaosRuntimeHelpersGetHashCode",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Monitor::TryEnter (SimpleForward)
        /// </summary>
        private static void RegisterMonitorTryEnter(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("Monitor", "TryEnter", ["System.Object", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosMonitorTryEnter",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// String::IndexOf (GenericShapeDescriptor -- handles various overloads)
        /// </summary>
        private static void RegisterStringIndexOf(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "IndexOf",
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
                            Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateInt32AbiSlot(),
                            EmptyRawArgumentIndices);
                    }
                    if (paramTypes.Count == 1 && paramTypes[0] == "System.String")
                    {
                        var srcIdx = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosStringIndexOf(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcIdx, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    if (paramTypes.Count >= 1 && paramTypes[0] == "System.Char")
                    {
                        // Probe surface: string.Empty.IndexOf(default(char)[,...]) == -1.
                        // The old fallback made every slot a reference slot — the char
                        // arrived as a handle and the body discarded everything returning
                        // 0 (assert expected -1).  Slot 0 is the receiver string, the
                        // char and the int offsets are Int32 values.
                        var slots = new List<AotCoreIrAbiSlotArtifact>
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        };
                        for (int i = 0; i < paramTypes.Count; i++)
                            slots.Add(CreateInt32AbiSlot());
                        var charParamSig = string.Join(", ", Enumerable.Range(0, slots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                        // (char) → start=0/count=whole; (char,int) → start=arg2; (char,int,int) → start/count
                        // from args; a trailing StringComparison (an Int32 enum) is ignored.
                        var startExpr = paramTypes.Count >= 2 && paramTypes[1] == "System.Int32"
                            ? "static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2)"
                            : "0";
                        var countExpr = paramTypes.Count >= 3 && paramTypes[2] == "System.Int32"
                            ? "static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3)"
                            : "-1";
                        var srcChar = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, charParamSig,
                        [
                            $"    return ChaosStringIndexOfChar(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), {startExpr}, {countExpr});",
                        ]);
                        return new GenericShapeResolution(srcChar, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(slots.ToArray()),
                            CreateInt32AbiSlot(),
                            new HashSet<int>(Enumerable.Range(0, slots.Count)));
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
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

    }
}
