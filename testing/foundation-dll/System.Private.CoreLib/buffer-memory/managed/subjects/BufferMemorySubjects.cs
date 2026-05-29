// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/buffer/memory
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class BufferMemorySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    public static void Subject_0()
    {
        try { Buffer.BlockCopy(new byte[8], 0, new byte[8], 0, 8); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    // [2] System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)
    // [3] System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)
    public static void Subject_3()
    {
        try { _ = (int)(Buffer.GetByte(new byte[4], 0)); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)
    // [5] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)
    // [6] System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    // [7] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)
    // [8] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)
    // [9] System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)
    public static void Subject_9()
    {
        try { Buffer.SetByte(new byte[4], 0, (byte)42); }
        catch { _exitCode = 1; }
    }

}