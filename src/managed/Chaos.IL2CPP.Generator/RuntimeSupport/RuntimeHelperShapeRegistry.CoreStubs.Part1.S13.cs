using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Thread::get_ThreadState
        /// </summary>
        private static void RegisterThreadgetThreadState(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "get_ThreadState", [],
                ShapeKind.SimpleForward, "chaos_thread_get_state",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Thread::get_IsThreadPoolThread
        /// </summary>
        private static void RegisterThreadgetIsThreadPoolThread(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Thread", "get_IsThreadPoolThread", [],
                ShapeKind.SimpleForward, "chaos_thread_is_threadpool",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Threading.Thread", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// DateTime
        /// </summary>
        private static void RegisterDateTime(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.DateTime", "get_UtcNow", [],
                ShapeKind.SimpleForward, "ChaosDatetimeGetUtcNow",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    TypeSubjectId = "System.Private.CoreLib/System.DateTime"
                },
                EmptyRawArgumentIndices);

        }

        /// <summary>
        /// TimeSpan
        /// </summary>
        private static void RegisterTimeSpan(RuntimeHelperShapeRegistry registry)
        {
            // FromMilliseconds: emit inline expression to avoid native dependency.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.TimeSpan",
                MethodName: "FromMilliseconds",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT64", symbol,
                        "double chaos_arg_0",
                    [
                        "    return static_cast<CHAOS_IL2CPP_INT64>(chaos_arg_0 * 10000.0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                        new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                        new HashSet<int> { 0 });
                }));

        }

        /// <summary>
        /// Reflection: Type
        /// </summary>
        private static void RegisterReflectionType(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Type", "GetTypeFromHandle", ["System.RuntimeTypeHandle"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromHandle",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "op_Inequality", ["System.Type", "System.Type"],
                ShapeKind.SimpleForward, "ChaosTypeInequality",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Type", "get_Assembly", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetAssembly",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetGenericTypeDefinition", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericTypeDefinition",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

            registry.Register("System.Type", "GetGenericArguments", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetGenericArguments",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

    }
}
