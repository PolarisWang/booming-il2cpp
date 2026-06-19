using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// HashCode::Combine<T,T> (GenericShapeDescriptor -- handles all instantiations)
        /// </summary>
        private static void RegisterHashCodeCombineT(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.HashCode",
                MethodName: "Combine",
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
                    if (paramTypes.Count == 2)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                        [
                            "    return ChaosHashCodeCombine2(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                            }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }
                    return null;
                }));

        }

        /// <summary>
        /// CompareInfo::Compare (SimpleForward stub)
        /// </summary>
        private static void RegisterCompareInfoCompare(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Globalization.CompareInfo", "Compare", ["System.String", "System.String"],
                ShapeKind.SimpleForward, "ChaosCompareInfoCompare",
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
        /// Assembly::GetExportedTypes (stub returning null)
        /// </summary>
        private static void RegisterAssemblyGetExportedTypes(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetExportedTypes", [],
                ShapeKind.SimpleForward, "ChaosReflectionAssemblyGetExportedTypes",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::MakeArrayType (SimpleForward stub)
        /// </summary>
        private static void RegisterTypeMakeArrayType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "MakeArrayType", [],
                ShapeKind.SimpleForward, "ChaosTypeMakeArrayType",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            // === (removed) FieldInfo/MethodBase::get_IsStatic generic stub — replaced by specific
            //     SimpleForward registrations above (MethodBase at line ~4889, FieldInfo at line ~5841).
            //     The specific Register() calls produce real C++ calls; this generic shape returned
            //     hardcoded 0 and ran first due to TryMatchGenericShape priority in the resolver.

        }

        /// <summary>
        /// ParameterInfo::GetOptionalCustomModifiers (stub returning null)
        /// </summary>
        private static void RegisterParameterInfoGetOptionalCustomModifiers(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "GetOptionalCustomModifiers", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetOptionalCustomModifiers",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
