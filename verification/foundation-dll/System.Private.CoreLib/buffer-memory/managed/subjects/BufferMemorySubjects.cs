// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/buffer/memory
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;

public static partial class BufferMemorySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    public static void Subject_0()
    {
        Buffer.BlockCopy(new byte[8], 0, new byte[8], 0, 8);
    }

    // [1] System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    public static void Subject_1()
    {
        // TODO: System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr) could not be auto-generated
    }

    // [2] System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)
    public static void Subject_2()
    {
        if (Buffer.ByteLength(Array.Empty<byte>()) != 0) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)
    public static void Subject_3()
    {
        if ((int)(Buffer.GetByte(new byte[4], 0)) != 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32) could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)
    public static void Subject_5()
    {
        // TODO: System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32) could not be auto-generated
    }

    // [6] System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    public static void Subject_6()
    {
        // TODO: System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr) could not be auto-generated
    }

    // [7] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)
    public static void Subject_7()
    {
        // TODO: System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64) could not be auto-generated
    }

    // [8] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)
    public static void Subject_8()
    {
        // TODO: System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64) could not be auto-generated
    }

    // [9] System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)
    public static void Subject_9()
    {
        Buffer.SetByte(new byte[4], 0, (byte)42);
    }

}