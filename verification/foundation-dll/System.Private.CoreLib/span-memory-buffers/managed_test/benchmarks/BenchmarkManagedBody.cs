// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/span/memory-buffers
using System;

namespace Chaos.Benchmark.span_memory_buffers
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)
        public static void System_Private_CoreLib_System_Span_1_get_Item_T_Ref_System_Int32()
        {
            BenchmarkChecksum += (int)(default(Span<byte>)[42]);
        }

        // System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)
        public static void System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32()
        {
            BenchmarkChecksum += ((default(Span<byte>).Slice(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Span_1_Slice_System_Span_1_System_Int32_System_Int32()
        {
            BenchmarkChecksum += ((default(Span<byte>).Slice(42, 42)).GetHashCode());
        }

        // System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)
        public static void System_Private_CoreLib_System_Span_1_ToArray_T_System_Int32()
        {
            BenchmarkChecksum += ((default(Span<byte>).ToArray(42)).Length);
        }

        // System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)
        public static void System_Private_CoreLib_System_Span_1_CopyTo_System_Void_System_Span_1()
        {
            BenchmarkChecksum += (int)(default(Span<byte>).CopyTo(default(Span<byte>)));
        }

        // System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()
        public static void System_Private_CoreLib_System_Span_1_get_Empty_System_Span_1()
        {
            BenchmarkChecksum += ((Span<byte>.Empty).GetHashCode());
        }

        // System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)
        public static void System_Private_CoreLib_System_ReadOnlySpan_1_get_Item_T_Ref_System_Int32()
        {
            BenchmarkChecksum += (int)(default(ReadOnlySpan<byte>)[42]);
        }

        // System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)
        public static void System_Private_CoreLib_System_ReadOnlySpan_1_Slice_System_ReadOnlySpan_1_System_Int32()
        {
            BenchmarkChecksum += ((default(ReadOnlySpan<byte>).Slice(42)).GetHashCode());
        }

        // System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()
        public static void System_Private_CoreLib_System_ReadOnlySpan_1_ToArray_T()
        {
            BenchmarkChecksum += ((default(ReadOnlySpan<byte>).ToArray()).Length);
        }

        // System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
        public static void System_Private_CoreLib_System_Memory_1_Slice_System_Memory_1_System_Int32()
        {
            BenchmarkChecksum += (int)(default(Memory<byte>).Slice(42));
        }

        // System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
        public static void System_Private_CoreLib_System_Memory_1_get_Span_System_Span_1()
        {
            BenchmarkChecksum += ((default(Memory<byte>).Span).GetHashCode());
        }

        // System.Private.CoreLib/System.Memory`1::ToArray:T[]()
        public static void System_Private_CoreLib_System_Memory_1_ToArray_T()
        {
            BenchmarkChecksum += ((default(Memory<byte>).ToArray()).Length);
        }

        // System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
        public static void System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_Ref_System_ReadOnlySpan_1()
        {
            BenchmarkChecksum += (int)(MemoryMarshal.GetReference(default(ReadOnlySpan<byte>)));
        }

        // System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
        public static void System_Private_CoreLib_System_Runtime_InteropServices_MemoryMarshal_GetReference_T_Ref_System_Span_1()
        {
            BenchmarkChecksum += (int)(MemoryMarshal.GetReference(default(Span<byte>)));
        }

    }
}
