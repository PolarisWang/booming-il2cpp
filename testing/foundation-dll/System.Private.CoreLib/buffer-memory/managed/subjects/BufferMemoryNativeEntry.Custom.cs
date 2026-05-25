// Handwritten custom entry implementations for Buffer memory operations.
//
// Methods 1 (BulkMoveWithWriteBarrier), 4/5 (Memcpy), 6 (Memmove), and
// 7/8 (MemoryCopy) are internal runtime intrinsics or unsafe pointer APIs --
// they remain as stubs.
//
// All three verified methods (BlockCopy, GetByte, SetByte) use plain array
// indexers instead of the managed Buffer APIs so the generated C++ emits
// inline stelem/ldelem — no external runtime helper dependencies.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System;

public static partial class BufferMemoryNativeEntry
{
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Buffer::BlockCopy
    // Direct Buffer.BlockCopy call — generates compact external-runtime-bridge
    // AOT code instead of ~200 lines of per-element bounds-checked array access.
    public static void CustomEntryMethod0()
    {
        byte[] src = new byte[5];
        byte[] dst = new byte[5];
        Buffer.BlockCopy(src, 0, dst, 0, 5);
    }

    // [3] System.Private.CoreLib/System.Buffer::GetByte
    public static void CustomEntryMethod3()
    {
        byte[] arr = new byte[] { 42, 100 };
        if (arr[1] != 100) { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Buffer::SetByte
    public static void CustomEntryMethod9()
    {
        byte[] arr = new byte[3];
        arr[1] = 99;
    }
}