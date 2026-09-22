// B1 — EqualityComparer<T> 泛型族。
//
// 背景：Assert.AreEqual<T> 比较两个集合/数组时，内部构造
// EqualityComparer<T>.Default 并调用其 Equals。这两个 callee 此前没有任何
// shape，每次调用落 catch-all 返回 0 —— get_Default 返回 null，随后对该
// null 调 Equals → NullReferenceException（reflection chunk 31 次 NRE 的
// 直接来源）。实测 52 个 callee：26 组 (get_Default, Equals)。
//
// 语义：BCL 的 EqualityComparer<T>.Default 对未重写 Equals 的 T 回退到
// 引用相等；AOT 描述符模型里这些 T（反射句柄、数组、自定义记录）都没有
// 自定义相等语义，所以按引用相等实现是正确的默认。Equals 转发到既有的
// ChaosRuntimeHelpersEquals（object_stubs.cpp）。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// EqualityComparer&lt;T&gt;.get_Default / Equals 的泛型注册。
        /// 实现见 object_stubs.cpp（转发 ChaosRuntimeHelpersEquals）。
        /// </summary>
        /// <remarks>
        /// 用 RegisterGeneric 而非 Register：T 的具体实参在 callee 里以
        /// 实例化形态出现（EqualityComparer&lt;System.Attribute&gt; 等），
        /// 逐个枚举会漏；prefix 匹配一次覆盖全部实例化。
        /// get_Default 是静态无参、Equals 是实例双参，两者的 receiver 形态
        /// 不同，所以分开注册。
        /// </remarks>
        private static void RegisterEqualityComparerFamily(RuntimeHelperShapeRegistry registry)
        {
            // EqualityComparer<T>.get_Default → EqualityComparer<T>
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.EqualityComparer",
                MethodName: "get_Default",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "    return ChaosEqualityComparerGetDefault();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        System.Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        EmptyRawArgumentIndices);
                }));

            // EqualityComparer<T>.Equals(T, T) → bool
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Collections.Generic.EqualityComparer",
                MethodName: "Equals",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_comparer, CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return ChaosEqualityComparerEquals(chaos_comparer, chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                        {
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1, 2 });
                }));
        }
    }
}
