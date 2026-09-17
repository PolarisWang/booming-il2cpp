using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Array::IndexOf (GenericShapeDescriptor -- calls ChaosArrayIndexOf for standard 2-param overload)
        /// </summary>
        private static void RegisterArrayIndexOf(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "IndexOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcIdx = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosArrayIndexOf(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcIdx, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosArrayIndexOf_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcFallback = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(srcFallback, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// String::LastIndexOf — char-overload family.  String.LastIndexOf had NO
        /// registration at all (only System.Array::LastIndexOf existed), so the char
        /// overloads hit the default external-runtime fallback: arity-correct INTPTR
        /// slots whose bodies discard every argument and return 0, losing to the probed
        /// -1.  Mirrors the String::IndexOf char branch in Part2.S5.
        /// </summary>
        private static void RegisterStringLastIndexOf(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.String",
                MethodName: "LastIndexOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count >= 1 && paramTypes[0] == "System.Char")
                    {
                        // Probe surface: string.Empty.LastIndexOf(default(char)[,...]) == -1.
                        // Slot 0 is the receiver string; char and int offsets are Int32 values.
                        var slots = new List<AotCoreIrAbiSlotArtifact>
                        {
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                        };
                        for (int i = 0; i < paramTypes.Count; i++)
                            slots.Add(CreateInt32AbiSlot());
                        var paramSig = string.Join(", ", Enumerable.Range(0, slots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                        // (char) → whole string; (char,int) → startIndex=arg2; (char,int,int) → startIndex/count.
                        // .NET LastIndexOf(value, startIndex) scans *backwards from* startIndex.
                        var startExpr = paramTypes.Count >= 2 && paramTypes[1] == "System.Int32"
                            ? "static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2)"
                            : "-1";
                        var countExpr = paramTypes.Count >= 3 && paramTypes[2] == "System.Int32"
                            ? "static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3)"
                            : "-1";
                        var srcChar = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                        [
                            $"    return ChaosStringLastIndexOfChar(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), {startExpr}, {countExpr});",
                        ]);
                        return new GenericShapeResolution(srcChar, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(slots.ToArray()),
                            CreateInt32AbiSlot(),
                            new HashSet<int>(Enumerable.Range(0, slots.Count)));
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSigAll = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSigAll,
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
        /// Array::LastIndexOf (GenericShapeDescriptor -- calls ChaosArrayLastIndexOf for standard 2-param overload)
        /// </summary>
        private static void RegisterArrayLastIndexOf(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Array",
                MethodName: "LastIndexOf",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 2)
                    {
                        var srcLastIdx = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return ChaosArrayLastIndexOf(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(srcLastIdx, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosArrayLastIndexOf_Inline");
                    }
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var srcLastIdxFb = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    return static_cast<CHAOS_IL2CPP_INT32>(0);",
                    ]);
                    return new GenericShapeResolution(srcLastIdxFb, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateInt32AbiSlot(),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Type::IsSubclassOf
        /// </summary>
        private static void RegisterTypeIsSubclassOf(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "IsSubclassOf", ["System.Type"],
                ShapeKind.SimpleForward, "ChaosReflectionIsSubclassOf",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Dictionary<K,V>::get_Count (GenericShapeDescriptor -- calls CollectionDictionaryGetCount)
        /// </summary>
        private static void RegisterDictionaryKV(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "get_Count",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return CollectionDictionaryGetCount(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Dictionary<K,V>::Add (GenericShapeDescriptor -- calls CollectionDictionaryAdd)
        /// </summary>
        private static void RegisterDictionaryKV_1(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.Dictionary",
                MethodName: "Add",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    CollectionDictionaryAdd(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));

        }

    }
}
