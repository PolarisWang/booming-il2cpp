using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Reflection: Type (additional)
        /// </summary>
        private static void RegisterReflectionType_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_TypeHandle", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetField", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetField",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Type::GetField with BindingFlags (GenericShapeDescriptor)
        /// </summary>
        private static void RegisterTypeGetFieldwith(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetField",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.FieldInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Type::GetEvent (all overloads via GenericShapeDescriptor)
        /// </summary>
        private static void RegisterTypeGetEvent(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetEvent",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.EventInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

        }

        /// <summary>
        /// Type::GetProperty (all overloads via GenericShapeDescriptor)
        /// </summary>
        private static void RegisterTypeGetProperty(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Type",
                MethodName: "GetProperty",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType) };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    var paramSig = string.Join(", ", Enumerable.Range(0, abiSlots.Count).Select(i => $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                    var voidExprs = string.Join("; ", Enumerable.Range(0, abiSlots.Count).Select(i => $"(void)chaos_arg_{i}"));
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
                    [
                        $"    {voidExprs};",
                        "    CHAOS_IL2CPP_FAIL();",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(abiSlots.ToArray()),
                        CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.PropertyInfo", AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int>(Enumerable.Range(0, abiSlots.Count)));
                }));

            registry.Register("System.Type", "GetMethod", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Type", "GetType", ["System.String"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeByName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetConstructors", ["System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetConstructors",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::GetConstructors (0-param fallback)
        /// </summary>
        private static void RegisterTypeGetConstructors(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetConstructors", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetConstructorsDefault",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetMethod", ["System.String", "System.Reflection.BindingFlags"],
                ShapeKind.SimpleForward, "ChaosReflectionGetMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    new AotCoreIrAbiSlotArtifact
                    {
                        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                        TypeShape = AotCoreIrTypeShapeKind.ValueType
                    },
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

    }
}
