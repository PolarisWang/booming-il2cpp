using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// MethodBase::get_IsStatic
        /// </summary>
        private static void RegisterMethodBasegetIsStatic(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodBase", "get_IsStatic", [],
                ShapeKind.SimpleForward, "chaos_reflection_get_is_static",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// ParameterInfo::get_Position
        /// </summary>
        private static void RegisterParameterInfogetPosition(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_Position", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParamPosition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Type::get_IsSealed
        /// </summary>
        private static void RegisterTypegetIsSealed(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "get_IsSealed", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetIsSealed",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Memory<T>::ToArray (GenericShapeDescriptor -- handles generic Memory<T>)
        /// </summary>
        private static void RegisterMemoryTToArray(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Memory",
                MethodName: "ToArray",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    (void)chaos_arg_0;",
                        "    return 0;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType)),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// String::StartsWith
        /// </summary>
        private static void RegisterStringStartsWith(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.String", "StartsWith", ["System.String"],
                ShapeKind.SimpleForward, "ChaosStringStartsWith",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

        }

    }
}
