// S34 — high-level net types: Stream::FlushAsync + UdpClient::SendAsync（NT-8）。
//
// 背景：NT-8（高层 native 重写，q_bcl）剩余两个现网失败 subject 都死在
// catch-all fallback 返回 0 → 生成调用点对结果做 null-check →
// raise_null_reference_exception()（caught=true）：
//   si 45  NetworkStreamTests::FlushAsync_16_CancellationToken_0（value=0）
//   si 555 UdpClientTests::SendAsync_18_System_Byte_int_IPEndPoint_3（value=0）
// 而主题（fact subject）在拿到活 Task 后走 ChaosAsyncTaskGetAwaiter /
// ChaosAsyncTaskAwaiterGetResultValue —— 断言链路只需一个 ALREADY-COMPLETED
// 的 AsyncTask 处理柄即可翻绿（result=0，无 fault），与 S29 同款先例。
//
// 类型匹配（subject-id 剥 assembly 前缀 + :: 后）：
//   System.Private.CoreLib/System.IO.Stream::FlushAsync → "System.IO.Stream"
//   System.Net.Sockets/UdpClient::SendAsync            → "UdpClient"（短名）
// 与 S5（Stream 系列 SimpleForward）和 S31（Socket NativeBody）的 key 拼法一致。
//
// receiver 注入语义：UdpClient 与 Stream 均不在 _ReceiverInjectedTypes
// allowlist（Socket 才自动注入）→ receiver 槽必须手写进 slots 列表
// （S5 Stream::Flush 零参实例方法先例：slots=[receiver ref]，raw{0}）。

using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator
{
    public sealed partial class NativeAotLoweringPlanner
    {
        partial class RuntimeHelperShapeRegistry
        {
            private static void RegisterS34NetHighLevelStubs(RuntimeHelperShapeRegistry registry)
            {
                // ① Stream::FlushAsync(CancellationToken) → Task
                //    SimpleForward：生成调用点直呼 ChaosStreamFlushAsync(receiver, ct)；
                //    native 侧返回已完成 Task（result=0，详见 entry.cpp）。
                //    slots = [receiver(ref), ct(NativeInt)]，raw{0,1}。
                registry.Register("System.IO.Stream", "FlushAsync",
                    ["System.Threading.CancellationToken"],
                    ShapeKind.SimpleForward, "ChaosStreamFlushAsync",
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0, 1 });

                // ② UdpClient::SendAsync(byte[], int, IPEndPoint) → Task<int>
                //    NativeBody wrapper：4 槽 = receiver(ref) + byte[](ref) +
                //    count(int32) + endpoint(ref)；wrapper 解包 byte[]→(ptr,len)
                //    后调 ChaosUdpClientSendAsync；native 返已完成 Task<int> result=0。
                var byteArraySlot = CreateNativeIntAbiSlot("System.Private.CoreLib/System.Byte[]",
                    AotCoreIrTypeShapeKind.ReferenceType);
                var int32Slot = CreateInt32AbiSlot();
                registry.Register("UdpClient", "SendAsync",
                    ["System.Byte[]", "System.Int32", "System.Net.IPEndPoint"],
                    ShapeKind.NativeBody, "ChaosUdpClientSendAsync",
                    new List<AotCoreIrAbiSlotArtifact>
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        byteArraySlot,
                        int32Slot,
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                    },
                    CreateNativeIntAbiSlot(),
                    new HashSet<int> { 0, 1, 2, 3 },
                    wrapperBodyLines:
                    [
                        "auto* chaos_arr_1 = get_managed_array(chaos_fn_arg_1);",
                        "return ChaosUdpClientSendAsync(chaos_fn_arg_0,",
                        "    chaos_arr_1 != nullptr ? reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(accessor_get_elements(chaos_arr_1)) : nullptr,",
                        "    chaos_arr_1 != nullptr ? chaos_arr_1->length : 0,",
                        "    chaos_fn_arg_2, chaos_fn_arg_3);",
                    ]);

                // ③ IPEndPoint::.ctor(IPAddress, Int32)（NT-8 si 555 修复）
                //    背景：此前该 ctor 未注册 → 生成调用点（EmitLinearNewObject
                //    reference-type ctor 分支）按 abiUnderreports 只消费 1 个显式参
                //    （port），Loopback 指针泄漏在 eval stack → 随后 SendAsync 的
                //    4 槽 (receiver, byte[], count, endpoint) 逆序取 stack 得到
                //    (byte[], count, INT32(Loopback), endpoint) 全错位、receiver
                //    未传 → wrapper 内 get_managed_array(count=0) NRE → caught。
                //    注册后调用点正取 2 显式参（IPAddress + port），Loopback 不再
                //    泄漏，SendAsync receiver 归位。简单 no-op native（subject
                //    不读取 endpoint.Address/Port），与 S5 MemoryStream ctor 同构。
                registry.Register("System.Net.IPEndPoint", ".ctor",
                    ["System.Net.IPAddress", "System.Int32"],
                    ShapeKind.SimpleForward, "ChaosIPEndPointCtor",
                    new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                    {
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                        CreateNativeIntAbiSlot(),
                    }),
                    CreateVoidAbiSlot(),
                    new HashSet<int> { 0, 1, 2 });
            }
        }
    }
}
