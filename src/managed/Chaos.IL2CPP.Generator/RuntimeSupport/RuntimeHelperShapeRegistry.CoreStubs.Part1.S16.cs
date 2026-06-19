using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Reflection: MethodInfo
        /// </summary>
        private static void RegisterReflectionMethodInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodInfo", "MakeGenericMethod", ["System.Type[]"],
                ShapeKind.SimpleForward, "ChaosReflectionMakeGenericMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Assembly::GetType(System.String,System.Boolean) overload
        /// </summary>
        private static void RegisterAssemblyGetTypeoverload(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetType", ["System.String", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromAssemblyBool",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Reflection: ParameterInfo
        /// </summary>
        private static void RegisterReflectionParameterInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameterName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Async: Task.Yield / YieldAwaitable
        /// </summary>
        private static void RegisterAsyncTaskYield(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Tasks.Task", "Yield", [],
                ShapeKind.SimpleForward, "chaos_async_yield_create",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable", "GetAwaiter", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_awaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "get_IsCompleted", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_is_completed",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_result",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Decimal
        /// </summary>
        private static void RegisterDecimal(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Decimal", ".ctor", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_decimal_ctor_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // Decimal::op_Explicit(Decimal) -> Int32 — forward to ChaosDecimalToInt32
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Decimal",
                MethodName: "op_Explicit",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosDecimalToInt32(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

        }

    }
}
