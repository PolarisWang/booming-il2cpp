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

        /// <summary>
        /// System.Reflection.AssemblyName::GetPublicKey / GetPublicKeyToken
        ///
        /// These were unregistered, so both fell through to the catch-all fallback
        /// (zero parameters, answers 0).  The subjects therefore saw null where the
        /// BCL returns a zero-length array, and
        /// `Assert.AreEqual(Array.Empty&lt;byte&gt;(), result)` failed — which is how
        /// they became the only two genuine assertion failures in the reflection
        /// chunk once the runner started stamping assertFailed.
        ///
        /// The native entries return ChaosArrayEmpty_Inline() for an unsigned
        /// assembly, matching .NET (verified: both accessors report len=0, and
        /// Array.Empty&lt;byte&gt;() is not null).
        /// </summary>
        private static void RegisterSystemReflectionAssemblyNamePublicKey(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.AssemblyName", "GetPublicKey", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameGetPublicKey",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Reflection.AssemblyName", "GetPublicKeyToken", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameGetPublicKeyToken",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Byte[]", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// System.Reflection.AssemblyName::ReferenceMatchesDefinition(Boolean, AssemblyName, AssemblyName)
        ///
        /// Static method — no receiver slot, two AssemblyName reference slots.
        /// This was unregistered, so it fell through to the catch-all fallback
        /// which (for this subject) was emitted with zero parameters and answered
        /// 0 — the probe-measured .NET answer for (null, null) is TRUE, so the
        /// subject failed its `Assert.AreEqual(true, ...)` (B7 si=58).  The
        /// native entry implements the measured semantics: (null, null) → true,
        /// one-sided null → false, else simple-name equality.
        /// </summary>
        private static void RegisterSystemReflectionAssemblyNameReferenceMatches(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.AssemblyName", "ReferenceMatchesDefinition",
                new[] { "System.Reflection.AssemblyName", "System.Reflection.AssemblyName" },
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyNameReferenceMatchesDefinition",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Reflection.AssemblyName", AotCoreIrTypeShapeKind.ReferenceType),
                }),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });
        }

    }
}
