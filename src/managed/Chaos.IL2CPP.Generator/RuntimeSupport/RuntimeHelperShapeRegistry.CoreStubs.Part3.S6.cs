using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Type::get_IsConstructedGenericType
        /// </summary>
        private static void RegisterTypegetIsConstructedGenericType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsConstructedGenericType", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsConstructedGeneric",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// MethodBase::get_IsVirtual
        /// </summary>
        private static void RegisterMethodBasegetIsVirtual(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodBase", "get_IsVirtual", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsVirtual",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// ParameterInfo::GetRequiredCustomModifiers
        /// </summary>
        private static void RegisterParameterInfoGetRequiredCustomModifiers(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "GetRequiredCustomModifiers", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetRequiredCustomModifiers",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_IsInterface
        /// </summary>
        private static void RegisterTypegetIsInterface(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsInterface", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsInterface",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// MemoryMarshal::GetReference<T> (GenericShapeDescriptor -- handles generic MemoryMarshal)
        /// </summary>
        private static void RegisterMemoryMarshalGetReferenceT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.InteropServices.MemoryMarshal",
                MethodName: "GetReference",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>(paramTypes.Count);
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType));
                    if (abiSlots.Count == 0)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            EmptyRawArgumentIndices);
                    }
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

    }
}
