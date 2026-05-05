// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/buffer/memory
using System;

namespace Chaos.Benchmark.buffer_memory
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(Buffer.BlockCopy(Array.Empty<byte>(), 42, Array.Empty<byte>(), 42, 42));
        }

        // System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
        public static void System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_Ref_System_Byte_Ref_System_UIntPtr()
        {
            BenchmarkChecksum += (int)(Buffer.BulkMoveWithWriteBarrier((byte)42, (byte)42, UIntPtr.Zero));
        }

        // System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)
        public static void System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array()
        {
            BenchmarkChecksum += Buffer.ByteLength(Array.Empty<byte>());
        }

        // System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)
        public static void System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32()
        {
            BenchmarkChecksum += (int)(Buffer.GetByte(Array.Empty<byte>(), 42));
        }

        // System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)
        public static void System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_Ptr_System_Byte_Ptr_System_Int32()
        {
            BenchmarkChecksum += (int)(Buffer.Memcpy((byte)42, (byte)42, 42));
        }

        // System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_Ptr_System_Int32_System_Byte_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(Buffer.Memcpy((byte)42, 42, new byte[] { 1, 2, 3 }, 42, 42));
        }

        // System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
        public static void System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_Ref_System_Byte_Ref_System_UIntPtr()
        {
            BenchmarkChecksum += (int)(Buffer.Memmove((byte)42, (byte)42, UIntPtr.Zero));
        }

        // System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)
        public static void System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_Ptr_System_Void_Ptr_System_Int64_System_Int64()
        {
            BenchmarkChecksum += (int)(Buffer.MemoryCopy(null!, null!, 42L, 42L));
        }

        // System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)
        public static void System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_Ptr_System_Void_Ptr_System_UInt64_System_UInt64()
        {
            BenchmarkChecksum += (int)(Buffer.MemoryCopy(null!, null!, 42uL, 42uL));
        }

        // System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)
        public static void System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte()
        {
            BenchmarkChecksum += (int)(Buffer.SetByte(Array.Empty<byte>(), 42, (byte)42));
        }

    }
}
