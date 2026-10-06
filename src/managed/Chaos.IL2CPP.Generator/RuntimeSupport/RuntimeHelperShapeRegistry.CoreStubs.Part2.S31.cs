// S31 — System.Net.Sockets.Socket::Send NativeBody wrappers（NT-2 wrapper 发射）。
//
// 背景：Socket 方法在 aot-core-ir 中为跨程序集 callee（System.Net.Sockets 程序集非
// 本模块编译），全部落 catch-all fallback（返回 0/Phase 3 抛异常）。NT-2 目标=
// NativeBody wrapper 发射 + 全参数转发：byte[]→(ptr,len)、out/ref→槽指针、
// 标量按 AotCoreIrAbiSlotArtifact 直传。
//
// 第一个 NativeBody subject 选 Socket::Send 两个变体（facts 中 Send_16/Send_17）：
// ① Send(System.Byte[],System.Int32,System.Int32,SocketFlags) → Int32
// ② Send(System.Byte[],System.Int32,System.Int32,SocketFlags,SocketError&) → Int32
//    （Send_17 系 = nightly 6 失败中 10057 目标，NT-4 翻绿对象）
//
// IR callee 形态（SocketsFull3 产物实证）：typeDisplayName 为短名 Socket
// （assembly 前缀被剥），参数 SocketFlags 为裸名、SocketError& 为全名带 & ——
// 注册 key 与此完全一致以命中 TryMatchShape 精确哈希（_entriesByShapeId）。
//
// NativeBody 语义：DirectNativeSymbol = wrapper 符号本身（CreateDefinitionFrom
// ShapeEntry L969-971 分支），调用点直接调 wrapper；wrapper body 在此做
// get_managed_array 解包 byte[]→(元素指针,长度)。native 实现（占位）见
// src/native/runtime-core/runtime_stubs/misc_stubs.cpp::ChaosSocketSendBytes*。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator
{
    public sealed partial class NativeAotLoweringPlanner
    {
        partial class RuntimeHelperShapeRegistry
        {
            private static void RegisterS31SocketSendStubs(RuntimeHelperShapeRegistry registry)
            {
                // byte[] 参数槽：NativeInt + ReferenceType（数组句柄），native 侧用
                // get_managed_array 解包；offset/size/flags 标量槽 Int32；
                // SocketError& 出参槽 = ByRefToValueType（Part2.S28 先例）。
                // rawArgumentIndices 标全参 raw → 调用点直传 ABI 槽值（receiver
                // 注入后 0=receiver 恒 raw，其余 +1）。
                var byteArraySlot = CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]",
                    AotCoreIrTypeShapeKind.ReferenceType);
                var int32Slot = CreateInt32AbiSlot();
                var errorByRefSlot = new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.ByRefToValueType
                };

                // ① Send(byte[], int, int, SocketFlags) → int
                //    4 显式参，receiver 注入后有效槽序：0=receiver,1=byte[],2=offset,
                //    3=size,4=flags。
                registry.Register("Socket", "Send",
                    ["System.Byte[]", "System.Int32", "System.Int32", "SocketFlags"],
                    ShapeKind.NativeBody, "ChaosSocketSendBytes",
                    new List<AotCoreIrAbiSlotArtifact> { byteArraySlot, int32Slot, int32Slot, int32Slot },
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1, 2, 3 },
                    wrapperBodyLines:
                    [
                        "auto* chaos_arr_1 = get_managed_array(chaos_fn_arg_1);",
                        "return ChaosSocketSendBytes(chaos_fn_arg_0,",
                        "    chaos_arr_1 != nullptr ? reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(chaos_arr_1)) : nullptr,",
                        "    chaos_arr_1 != nullptr ? chaos_arr_1->length : 0,",
                        "    chaos_fn_arg_2, chaos_fn_arg_3, chaos_fn_arg_4);",
                    ]);

                // ② Send(byte[], int, int, SocketFlags, SocketError&) → int
                //    5 显式参，receiver 注入后有效槽序：0=receiver,1=byte[],2=offset,
                //    3=size,4=flags,5=error（ByRef 槽=INTPTR → reinterpret_cast<INT32*>）。
                registry.Register("Socket", "Send",
                    ["System.Byte[]", "System.Int32", "System.Int32", "SocketFlags", "System.Net.Sockets.SocketError&"],
                    ShapeKind.NativeBody, "ChaosSocketSendBytesError",
                    new List<AotCoreIrAbiSlotArtifact> { byteArraySlot, int32Slot, int32Slot, int32Slot, errorByRefSlot },
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1, 2, 3, 4 },
                    wrapperBodyLines:
                    [
                        "auto* chaos_arr_1 = get_managed_array(chaos_fn_arg_1);",
                        "return ChaosSocketSendBytesError(chaos_fn_arg_0,",
                        "    chaos_arr_1 != nullptr ? reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(chaos_arr_1)) : nullptr,",
                        "    chaos_arr_1 != nullptr ? chaos_arr_1->length : 0,",
                        "    chaos_fn_arg_2, chaos_fn_arg_3, chaos_fn_arg_4,",
                        "    reinterpret_cast<CHAOS_IL2CPP_INT32*>(chaos_fn_arg_5));",
                    ]);
            }
        }
    }
}
