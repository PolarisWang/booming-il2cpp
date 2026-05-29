// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/buffer/memory
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class BufferMemoryPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    // [2] System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)
    // [3] System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)
    // [5] System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)
    // [6] System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)
    // [7] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)
    // [8] System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)
    // [9] System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: Subject_3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: Subject_9(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}