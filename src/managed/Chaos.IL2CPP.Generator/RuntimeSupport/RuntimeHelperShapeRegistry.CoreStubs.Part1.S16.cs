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

            // Decimal.FromOACurrency(long) -> Decimal — forward to a real native that
            // builds a DecimalCarrier*. Avoids the codegen 0-arg catch-all stub (the Scalar
            // int64/arg is dropped there, returning a null carrier), so the fact passes.
            registry.Register("System.Decimal", "FromOACurrency", ["System.Int64"],
                ShapeKind.SimpleForward, "ChaosDecimalFromOACurrency",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

            // Decimal.CreateChecked/CreateSaturating/CreateTruncating(int) -> Decimal.
            // Generic int→Decimal conversion. SimpleForward didn't match the generic
            // instantiation (0-arg catch-all stub wall), so route as inline shapes that
            // emit a direct ChaosDecimalFromInt32 call at the call site.
            RegisterDecimalFromInt32Inline(registry, "CreateChecked");
            RegisterDecimalFromInt32Inline(registry, "CreateSaturating");
            RegisterDecimalFromInt32Inline(registry, "CreateTruncating");

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

            // Decimal arithmetic + type rounding — DecimalCarrier* in/out. SimpleForward to
            // the runtime natives so the ATG wrapper AOT-lowers these to real 1/2-arg native
            // calls (instead of the 0-arg catch-all) and returns real Decimal carriers.
            RegisterDecimalBinary(registry, "Add", "ChaosDecimalAdd");
            RegisterDecimalBinary(registry, "Subtract", "ChaosDecimalSubtract");
            RegisterDecimalBinary(registry, "Multiply", "ChaosDecimalMultiply");
            RegisterDecimalBinary(registry, "Divide", "ChaosDecimalDivide");
            RegisterDecimalBinary(registry, "Remainder", "ChaosDecimalRemainder");
            RegisterDecimalBinary(registry, "CopySign", "ChaosDecimalIdentity");
            RegisterDecimalBinary(registry, "MaxMagnitude", "ChaosDecimalIdentity");
            RegisterDecimalBinary(registry, "MinMagnitude", "ChaosDecimalIdentity");
            RegisterDecimalUnary(registry, "Negate", "ChaosDecimalNegate");
            RegisterDecimalUnary(registry, "Ceiling", "ChaosMathDecimalCeiling");
            RegisterDecimalUnary(registry, "Floor", "ChaosMathDecimalFloor");
            RegisterDecimalUnary(registry, "Round", "ChaosMathDecimalRound");
            RegisterDecimalUnary(registry, "Truncate", "ChaosMathDecimalTruncate");

        }

        /// <summary>Register a Decimal→Decimal binary (2 carriers in, 1 carrier out) static method.</summary>
        private static void RegisterDecimalBinary(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            registry.Register("System.Decimal", methodName, ["System.Decimal", "System.Decimal"],
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });
        }

        /// <summary>Register a Decimal→Decimal unary (1 carrier in, 1 carrier out) static method.</summary>
        private static void RegisterDecimalUnary(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            registry.Register("System.Decimal", methodName, ["System.Decimal"],
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });
        }

        /// <summary>Register Decimal.CreateChecked/Saturating/Truncating T=int as an inline
        /// direct ChaosDecimalFromInt32 call. These generic methods fall into the codegen
        /// 0-arg catch-all stub (dropping the int arg) via the external-runtime dispatch;
        /// routing inline (Priority-1, bypassing that dispatch) emits the real native call.</summary>
        private static void RegisterDecimalFromInt32Inline(RuntimeHelperShapeRegistry registry,
            string methodName)
        {
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Decimal",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    // Only match the single-param T=int instantiation.
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32")
                        return null;
                    // {0} is the raw int32 carrier; ChaosDecimalFromInt32 builds a Decimal carrier.
                    return "ChaosDecimalFromInt32({0})";
                }));
        }

    }
}
