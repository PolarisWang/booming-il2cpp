// S33 — System.Net.Sockets.Socket::Receive NativeBody wrappers（NT-4 翻绿 si 132/133）。
//
// 背景：与 S31 Send 完全同构 — Receive 位于 System.Net.Sockets 跨程序集 callee，
// 此前落 catch-all fallback（返回 0 且不写 out SocketError 槽 → Assert.AreEqual
// ((SocketError)10057, __ref) 失败 → assertFailed）。S33 注册两个变体：
// ① Receive(System.Byte[],System.Int32,System.Int32,SocketFlags) → Int32
// ② Receive(System.Byte[],System.Int32,System.Int32,SocketFlags,SocketError&) → Int32
//    （Receive_28_3/4 = si 132/133，夜间 fact：Assert.AreEqual(0,result) +
//     Assert.AreEqual((SocketError)10057,__ref)）。
//
// NativeBody 语义：DirectNativeSymbol = wrapper 符号本身；wrapper body 用
// get_managed_array 解包 byte[]→(元素指针,长度) 后转发 ChaosSocketReceiveBytes*；
// native 实现（Layer A entry.cpp → Layer B socket_ops.cpp::SocketReceiveBytes）
// 未连接 socket → *error=NotConnected(10057) 且 return 0（与 .NET WSAENOTCONN 契约一致）。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator
{
    public sealed partial class NativeAotLoweringPlanner
    {
        partial class RuntimeHelperShapeRegistry
        {
            private static void RegisterS33SocketReceiveStubs(RuntimeHelperShapeRegistry registry)
            {
                // 与 S31 相同的槽模板：byte[]=NativeInt+ReferenceType，标量=Int32，
                // SocketError&=ByRefToValueType。
                var byteArraySlot = CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]",
                    AotCoreIrTypeShapeKind.ReferenceType);
                var int32Slot = CreateInt32AbiSlot();
                var errorByRefSlot = new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.ByRefToValueType
                };

                // ① Receive(byte[], int, int, SocketFlags) → int
                registry.Register("Socket", "Receive",
                    ["System.Byte[]", "System.Int32", "System.Int32", "SocketFlags"],
                    ShapeKind.NativeBody, "ChaosSocketReceiveBytes",
                    new List<AotCoreIrAbiSlotArtifact> { byteArraySlot, int32Slot, int32Slot, int32Slot },
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1, 2, 3 },
                    wrapperBodyLines:
                    [
                        "auto* chaos_arr_1 = get_managed_array(chaos_fn_arg_1);",
                        "return ChaosSocketReceiveBytes(chaos_fn_arg_0,",
                        "    chaos_arr_1 != nullptr ? const_cast<CHAOS_IL2CPP_UINT8*>(reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(chaos_arr_1))) : nullptr,",
                        "    chaos_arr_1 != nullptr ? chaos_arr_1->length : 0,",
                        "    chaos_fn_arg_2, chaos_fn_arg_3, chaos_fn_arg_4);",
                    ]);

                // ② Receive(byte[], int, int, SocketFlags, SocketError&) → int
                registry.Register("Socket", "Receive",
                    ["System.Byte[]", "System.Int32", "System.Int32", "SocketFlags", "System.Net.Sockets.SocketError&"],
                    ShapeKind.NativeBody, "ChaosSocketReceiveBytesError",
                    new List<AotCoreIrAbiSlotArtifact> { byteArraySlot, int32Slot, int32Slot, int32Slot, errorByRefSlot },
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0, 1, 2, 3, 4 },
                    wrapperBodyLines:
                    [
                        "auto* chaos_arr_1 = get_managed_array(chaos_fn_arg_1);",
                        "return ChaosSocketReceiveBytesError(chaos_fn_arg_0,",
                        "    chaos_arr_1 != nullptr ? const_cast<CHAOS_IL2CPP_UINT8*>(reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(chaos_arr_1))) : nullptr,",
                        "    chaos_arr_1 != nullptr ? chaos_arr_1->length : 0,",
                        "    chaos_fn_arg_2, chaos_fn_arg_3, chaos_fn_arg_4,",
                        "    reinterpret_cast<CHAOS_IL2CPP_INT32*>(chaos_fn_arg_5));",
                    ]);
            }
        }
    }
}
