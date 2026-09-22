// T7 — Type/Module 成员查询族的 BindingFlags 变体 shape 注册。
//
// 背景：reflection chunk 的 23 项失败全部是「调用了未注册的 callee → 落
// catch-all → 返回 0」。这批 callee 是 Type/Module 上「列出成员」的
// BindingFlags 重载，其 native 实现早已存在（reflection/type_stubs.cpp、
// members.cpp、module.cpp），只是没有 shape 条目。
//
// 语义依据：AOT 描述符模型里，每个类型只收录它自身的成员，因此
// 「带 BindingFlags 的查询」与「无参查询」在描述符模型下是同一件事 ——
// 无参版本已经返回声明的成员集合（见 type_stubs.cpp 的 Declared* 注释）。
// 所以这些重载直接转发到已有的无参 native 符号；flags 作为显式参数占一个
// ABI 槽传入，native 侧忽略它（签名只取 receiver）。
//
// 参数形态说明：IR 里这些 callee 的签名把返回类型写在方法名中，例如
// `System.Type::GetNestedTypes:System.Type[](System.Reflection.BindingFlags)`。
// 注册键是 (类型, 方法名, 显式参数列表)；ABI 槽只列显式参数，receiver 由
// CreateDefinitionFromShapeEntry 的注入机制提供。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// System.Type / System.Reflection.Module 的 BindingFlags 成员查询重载。
        /// native 实现在 reflection/type_stubs.cpp 等，此处只补 shape 条目。
        /// </summary>
        private static void RegisterMemberQueryBindingFlagsOverloads(RuntimeHelperShapeRegistry registry)
        {
            // ── Type::GetNestedTypes(BindingFlags) → Type[] ──
            RegisterFlagsQuery(registry, "System.Type", "GetNestedTypes",
                "System.Reflection.BindingFlags", "ChaosReflectionGetNestedTypesFlags");

            // ── Type::GetEvents(BindingFlags) → EventInfo[] ──
            RegisterFlagsQuery(registry, "System.Type", "GetEvents",
                "System.Reflection.BindingFlags", "ChaosTypeGetEventsFlags");

            // ── Type::GetMembers(BindingFlags) → MemberInfo[] ──
            // Reuses the pre-existing ManagedArray variant (members.cpp), which
            // already takes (receiver, binding_flags) and wraps the flat buffer.
            RegisterFlagsQuery(registry, "System.Type", "GetMembers",
                "System.Reflection.BindingFlags", "ChaosReflectionGetMembersManagedArray");

            // ── Module::GetMethods([BindingFlags]) → MethodInfo[] ──
            RegisterFlagsQuery(registry, "System.Reflection.Module", "GetMethods",
                "System.Reflection.BindingFlags", "ChaosReflectionModuleGetMethodsFlags");
            RegisterFlagsQuery(registry, "System.Reflection.Module", "GetMethods",
                null, "ChaosReflectionModuleGetMethodsArray");

            // ── Module::GetFields([BindingFlags]) → FieldInfo[] ──
            RegisterFlagsQuery(registry, "System.Reflection.Module", "GetFields",
                "System.Reflection.BindingFlags", "ChaosReflectionModuleGetFieldsFlags");
            RegisterFlagsQuery(registry, "System.Reflection.Module", "GetFields",
                null, "ChaosReflectionModuleGetFieldsArray");
        }

        /// <summary>
        /// 注册一个「成员查询」shape。flagsParam 为 null 表示无参重载。
        /// </summary>
        /// <remarks>
        /// receiver 槽**显式声明**在这里（与 RegisterTypeGetMethods 的既有写法
        /// 一致），而不是依赖 CreateDefinitionFromShapeEntry 的注入机制 ——
        /// System.Type/Module 不在 _ReceiverInjectedTypes 白名单里，而把它们
        /// 加进白名单会让**已有 shape** 的 Type 方法双重注入（C2660 遍地）。
        /// 白名单是类型级的，无法区分"该 shape 是否已自带 receiver"。
        /// </remarks>
        private static void RegisterFlagsQuery(
            RuntimeHelperShapeRegistry registry,
            string typeName,
            string methodName,
            string? flagsParam,
            string nativeSymbol)
        {
            var paramTypes = flagsParam is null
                ? System.Array.Empty<string>()
                : new[] { flagsParam };
            // Slot 0 is the receiver (the Type/Module instance); any explicit
            // parameter follows it.  The natives take the receiver first for the
            // same reason.
            var abis = new List<AotCoreIrAbiSlotArtifact>(2)
            {
                CreateNativeIntAbiSlot(typeName, AotCoreIrTypeShapeKind.ReferenceType),
            };
            var rawArgs = new HashSet<int> { 0 };
            if (flagsParam is not null)
            {
                abis.Add(CreateInt32AbiSlot());
                rawArgs.Add(1);
            }

            registry.Register(typeName, methodName, paramTypes,
                ShapeKind.SimpleForward, nativeSymbol,
                abis,
                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                rawArgs);
        }
    }
}
