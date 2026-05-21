// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/read/flush/results
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ReadFlushResultsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCanceled:System.Boolean()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCompleted:System.Boolean()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.FlushResult::.ctor:System.Void(System.Boolean,System.Boolean)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Buffer:System.Buffers.ReadOnlySequence{System.Byte}()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCanceled:System.Boolean()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCompleted:System.Boolean()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.ReadResult::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Boolean)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}