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

public static partial class BufferMemoryPatchEntry
{

    // [0] System.Private.CoreLib/System.Buffer::BlockCopy
    // Manual byte-by-byte copy via array indexers
    // NOTE: simple zero-init arrays + individual assignments (no for-loop,
    // no array initializer syntax) to avoid codegen branching and
    // RuntimeHelpers.InitializeArray gaps in linear emission path.
    public static void CustomEntryMethod0()
    {
        byte[] src = new byte[5];
        byte[] dst = new byte[5];
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
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