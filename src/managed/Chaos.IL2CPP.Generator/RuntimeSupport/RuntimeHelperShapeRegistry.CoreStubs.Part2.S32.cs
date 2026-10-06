// S32 — Assert.AreEqual<SocketError> value-type safe wrapper（NT-4 翻绿 si 91/92）。
//
// 背景：Send_17_SocketError_3/4 subject 体调用 Assert.AreEqual<SocketError>(expected,
// actual, message)（值类型参数）。生成器为 AreEqual<T> 发射共享 generic 体
// （__generic pc-dispatch 状态机），该状态机把参数当对象指针 —— case 12/13 的
// is-string 检查 reinterpret_cast<ThinLockableHeader*>(rawInt) + chaos_object_get_type_info
// 对原始整值 10057 解引用 → 0xc0000005 AV。而 assert1 AreEqual(System.Int32,...)(0,result)
// 有专用直线体（按值比较）→ 通过。
//
// 根因链（重检续④-⑦封闭）：generic 定义体里 box !!0 的子题是
// "Chaos.TestFramework.Sdk/!!0"（CreateSimpleTypeIdentity(CurrentAssemblyName,...)），
// 实例化投影仅文本替换 "!!0"→"System.Net.Sockets.SocketError"、前缀保留 →
// "Chaos.TestFramework.Sdk/System.Net.Sockets.SocketError" → AotCoreIrLowering box case
// managedTypes.TryGetValue 未命中（System.Net.Sockets.dll 不在世界中）→
// ResolveTypeShape 回落（IsKnownValueTypeIdentity 不认识）→ ReferenceType 形 →
// EmitLinearBox 身份直通不分配 → 裸值 10057 到达 isinst 解引用 → AV。
//
//  注：B1 ChaosEqualityComparerEquals 为原始指针比较（left==right），即使 box 修复
//  也无法翻绿（两个不同箱值永不等）—— 故 box 修复方向被否定。
//
// 修复：GenericShapeDescriptor 在 TryCreateExternalRuntimeHelperDefinition 的
// TryMatchGenericShape 阶段截获（先于 _methodsBySubjectId 早退），callee=
// "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual<System.Net.Sockets.
// SocketError>:System.Void(System.Net.Sockets.SocketError,System.Net.Sockets.SocketError,
// System.String)" → typeDisplayName="Chaos.TestFramework.Assert"、
// methodName="AreEqual<System.Net.Sockets.SocketError>" → 角括号回退
// （entry.MethodName="AreEqual" 前缀 + typeDisplayName.StartsWith 验证）→
// typeArgs=["System.Net.Sockets.SocketError"]。命中时返回 NativeBody 风格 wrapper：
// 两个 INT32 按值比较，不等时复制直线体 Fail 序列（s_exitCode=1 + "[ASSERT FAIL] "
// 控制台 + AssertionException ctor + CHAOS_EH_THROW）—— 真实断言语义，完全绕开
// __generic 共享体（case 12 解引用 AV）。EqualityComparer<T>.Default.Equals 对
// SocketError（enum）等价于按值 ==，语义一致。
//
// 防护：typeArgs.Count != 1 或 typeArgs[0] != "System.Net.Sockets.SocketError"（含
// 非 generic 空 list：AreEqual(Int32,...) 直线体匹配）一律返回 null 落回旧路，
// 不影响其他重载/实例化（AreEqual<IAsyncResult> 等）。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator
{
    public sealed partial class NativeAotLoweringPlanner
    {
        partial class RuntimeHelperShapeRegistry
        {
            private static void RegisterS32AssertSocketErrorAreEqualStubs(RuntimeHelperShapeRegistry registry)
            {
                registry.RegisterGeneric(new RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "Chaos.TestFramework.Assert",
                    MethodName: "AreEqual",
                    Resolver: static (planner, callee, typeArgs) =>
                    {
                        if (typeArgs.Count != 1 || typeArgs[0] != "System.Net.Sockets.SocketError")
                        {
                            return null;
                        }

                        var symbol = NativeAotLoweringPlanner.GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                            new string[]
                            {
                                    "if (chaos_arg_0 != chaos_arg_1)",
                                    "{",
                                    "    CHAOS_IL2CPP_INTPTR chaos_msg = chaos_arg_2;",
                                    "    if (chaos_msg == 0)",
                                    "    {",
                                    "        CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_msg_storage{};",
                                    "        CHAOS_IL2CPP_INTPTR chaos_handler = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_msg_storage[1]);",
                                    "        chaos_default_interpolated_string_handler_reset(chaos_handler, static_cast<CHAOS_IL2CPP_INT32>(15), static_cast<CHAOS_IL2CPP_INT32>(2));",
                                    "        chaos_default_interpolated_string_handler_append_string(chaos_handler, CHAOS_IL2CPP_STRING_ID(\"Expected \"));",
                                    "        chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_DefaultInterpolatedStringHandler__AppendFormatted_System_Int32__System_Void_System_Int32_(chaos_handler, static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_0));",
                                    "        chaos_default_interpolated_string_handler_append_string(chaos_handler, CHAOS_IL2CPP_STRING_ID(\", got \"));",
                                    "        chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_DefaultInterpolatedStringHandler__AppendFormatted_System_Int32__System_Void_System_Int32_(chaos_handler, static_cast<CHAOS_IL2CPP_INTPTR>(chaos_arg_1));",
                                    "        chaos_msg = chaos_default_interpolated_string_handler_to_string_and_clear(chaos_handler);",
                                    "    }",
                                    "    chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = static_cast<CHAOS_IL2CPP_INT32>(1);",
                                    "    CHAOS_IL2CPP_INTPTR chaos_console = ChaosConsoleGetError();",
                                    "    CHAOS_IL2CPP_INTPTR chaos_line = ChaosStringConcat2(CHAOS_IL2CPP_STRING_ID(\"[ASSERT FAIL] \"), chaos_msg);",
                                    "    if (chaos_console == 0)",
                                    "    {",
                                    "        chaos_runtime_get_abi_v0()->raise_null_reference_exception();",
                                    "    }",
                                    "    ChaosTextWriterWriteLineStr(chaos_console, chaos_line);",
                                    "    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException, {});",
                                    "    chaos_object->header.type_info = chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException.AsTypeInfoHot();",
                                    "    Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_msg);",
                                    "    CHAOS_EH_THROW(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));",
                                    "}",
                                    "return;",
                            });

                        return new RuntimeHelperShapeRegistry.GenericShapeResolution(
                            src,
                            symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                            }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1, 2 },
                            DirectNativeSymbol: symbol);
                    }));
            }
        }
    }
}
