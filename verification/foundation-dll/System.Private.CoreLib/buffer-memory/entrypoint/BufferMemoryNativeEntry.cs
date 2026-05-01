// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/buffer/memory
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class BufferMemoryNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32) could not be auto-generated
        return 0;
    }

    // [1] System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    static int Method1()
    {
    var refLocal_0 = (byte)42;
    var refLocal_1 = (byte)42;
        Buffer.BulkMoveWithWriteBarrier(ref refLocal_0, ref refLocal_1, UIntPtr.Zero);
        return (int)refLocal_0 ^ refLocal_1;
    }

    // [2] System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)
    static int Method2()
    {
        return Buffer.ByteLength(Array.Empty<byte>());
    }

    // [3] System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)
    static int Method3()
    {
        // TODO: System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32) could not be auto-generated
        return 0;
    }

    // [4] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)
    static int Method4()
    {
        // TODO: System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32) could not be auto-generated
        return 0;
    }

    // [5] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)
    static int Method5()
    {
        // TODO: System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32) could not be auto-generated
        return 0;
    }

    // [6] System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    static int Method6()
    {
    var refLocal_0 = (byte)42;
    var refLocal_1 = (byte)42;
        Buffer.Memmove(ref refLocal_0, ref refLocal_1, UIntPtr.Zero);
        return (int)refLocal_0 ^ refLocal_1;
    }

    // [7] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)
    static int Method7()
    {
        // TODO: System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64) could not be auto-generated
        return 0;
    }

    // [8] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)
    static int Method8()
    {
        // TODO: System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64) could not be auto-generated
        return 0;
    }

    // [9] System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)
    static int Method9()
    {
        // TODO: System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte) could not be auto-generated
        return 0;
    }

}