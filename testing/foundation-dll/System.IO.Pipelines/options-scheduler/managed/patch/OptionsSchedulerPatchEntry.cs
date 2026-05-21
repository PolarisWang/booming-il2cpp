// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/options/scheduler
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class OptionsSchedulerPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Default:System.IO.Pipelines.PipeOptions()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_MinimumSegmentSize:System.Int32()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_PauseWriterThreshold:System.Int64()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ReaderScheduler:System.IO.Pipelines.PipeScheduler()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ResumeWriterThreshold:System.Int64()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_UseSynchronizationContext:System.Boolean()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_WriterScheduler:System.IO.Pipelines.PipeScheduler()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.IO.Pipelines.PipeScheduler,System.IO.Pipelines.PipeScheduler,System.Int64,System.Int64,System.Int32,System.Boolean)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Inline:System.IO.Pipelines.PipeScheduler()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_ThreadPool:System.IO.Pipelines.PipeScheduler()
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Schedule:System.Void(System.Action{System.Object},System.Object)
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_BufferSize:System.Int32()
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_LeaveOpen:System.Boolean()
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_MinimumReadSize:System.Int32()
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static int Subject_15()
    {
        return unchecked((int)(0xB0000000u + 15));
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_UseZeroByteReads:System.Boolean()
    public static int Subject_16()
    {
        return unchecked((int)(0xB0000000u + 16));
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean)
    public static int Subject_17()
    {
        return unchecked((int)(0xB0000000u + 17));
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean,System.Boolean)
    public static int Subject_18()
    {
        return unchecked((int)(0xB0000000u + 18));
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_LeaveOpen:System.Boolean()
    public static int Subject_19()
    {
        return unchecked((int)(0xB0000000u + 19));
    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_MinimumBufferSize:System.Int32()
    public static int Subject_20()
    {
        return unchecked((int)(0xB0000000u + 20));
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static int Subject_21()
    {
        return unchecked((int)(0xB0000000u + 21));
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Boolean)
    public static int Subject_22()
    {
        return unchecked((int)(0xB0000000u + 22));
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
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: Subject_12(); break;
                case 13: Subject_13(); break;
                case 14: Subject_14(); break;
                case 15: Subject_15(); break;
                case 16: Subject_16(); break;
                case 17: Subject_17(); break;
                case 18: Subject_18(); break;
                case 19: Subject_19(); break;
                case 20: Subject_20(); break;
                case 21: Subject_21(); break;
                case 22: Subject_22(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}