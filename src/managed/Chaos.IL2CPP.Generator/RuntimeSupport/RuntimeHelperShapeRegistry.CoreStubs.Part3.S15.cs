using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Reflection.Assembly::get_EntryPoint (stub returning null)
        /// </summary>
        private static void RegisterSystemReflectionAssembly_2(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "get_EntryPoint", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetEntryPoint",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.Assembly::GetManifestResourceStream (stub returning null)
        /// </summary>
        private static void RegisterSystemReflectionAssembly_3(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.Assembly",
                MethodName: "GetManifestResourceStream",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
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

        /// <summary>
        /// System.Reflection.Assembly::GetModule (stub returning null)
        /// </summary>
        private static void RegisterSystemReflectionAssembly_4(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Reflection.Assembly",
                MethodName: "GetModule",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var abiSlots = new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)
                    };
                    foreach (var _ in paramTypes)
                        abiSlots.Add(CreateInt32AbiSlot());
                    if (abiSlots.Count == 1)
                    {
                        var src0 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    (void)chaos_arg_0;",
                            "    return 0;",
                        ]);
                        return new GenericShapeResolution(src0, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            new HashSet<int> { 0 });
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

        /// <summary>
        /// System.Reflection.AssemblyName::get_CultureInfo (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemReflectionAssemblyName(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.AssemblyName", "get_CultureInfo", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameGetCultureInfo",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.AssemblyName::get_Version (SimpleForward stub)
        /// </summary>
        private static void RegisterSystemReflectionAssemblyName_1(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.AssemblyName", "get_Version", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameGetVersion",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
