// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/options-scheduler
// Assembly: System.IO.Pipelines
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Pipelines;
using System.Linq;

public static partial class OptionsSchedulerNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Default:System.IO.Pipelines.PipeOptions()
    public static void Method0()
    {
        try { if ((int)(PipeOptions.Default) != (int)(PipeOptions.Default)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_MinimumSegmentSize:System.Int32()
    public static void Method1()
    {
        try { if (PipeOptions.MinimumSegmentSize != PipeOptions.MinimumSegmentSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_PauseWriterThreshold:System.Int64()
    public static void Method2()
    {
        try { if ((int)(PipeOptions.PauseWriterThreshold) != (int)(PipeOptions.PauseWriterThreshold)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Method3()
    {
        try { if ((int)(PipeOptions.Pool) != (int)(PipeOptions.Pool)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ReaderScheduler:System.IO.Pipelines.PipeScheduler()
    public static void Method4()
    {
        try { if ((int)(PipeOptions.ReaderScheduler) != (int)(PipeOptions.ReaderScheduler)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ResumeWriterThreshold:System.Int64()
    public static void Method5()
    {
        try { if ((int)(PipeOptions.ResumeWriterThreshold) != (int)(PipeOptions.ResumeWriterThreshold)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_UseSynchronizationContext:System.Boolean()
    public static void Method6()
    {
        try { if (((PipeOptions.UseSynchronizationContext) ? 1 : 0) != ((PipeOptions.UseSynchronizationContext) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_WriterScheduler:System.IO.Pipelines.PipeScheduler()
    public static void Method7()
    {
        try { if ((int)(PipeOptions.WriterScheduler) != (int)(PipeOptions.WriterScheduler)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.IO.Pipelines.PipeScheduler,System.IO.Pipelines.PipeScheduler,System.Int64,System.Int64,System.Int32,System.Boolean)
    public static void Method8()
    {
        try { new PipeOptions(null!, null!, null!, 42L, 42L, 42, true); }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Inline:System.IO.Pipelines.PipeScheduler()
    public static void Method9()
    {
        try { if ((int)(PipeScheduler.Inline) != (int)(PipeScheduler.Inline)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_ThreadPool:System.IO.Pipelines.PipeScheduler()
    public static void Method10()
    {
        try { if ((int)(PipeScheduler.ThreadPool) != (int)(PipeScheduler.ThreadPool)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Schedule:System.Void(System.Action{System.Object},System.Object)
    public static void Method11()
    {
        try { PipeScheduler.Schedule(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_BufferSize:System.Int32()
    public static void Method12()
    {
        try { if (StreamPipeReaderOptions.BufferSize != StreamPipeReaderOptions.BufferSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_LeaveOpen:System.Boolean()
    public static void Method13()
    {
        try { if (((StreamPipeReaderOptions.LeaveOpen) ? 1 : 0) != ((StreamPipeReaderOptions.LeaveOpen) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_MinimumReadSize:System.Int32()
    public static void Method14()
    {
        try { if (StreamPipeReaderOptions.MinimumReadSize != StreamPipeReaderOptions.MinimumReadSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Method15()
    {
        try { if ((int)(StreamPipeReaderOptions.Pool) != (int)(StreamPipeReaderOptions.Pool)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_UseZeroByteReads:System.Boolean()
    public static void Method16()
    {
        try { if (((StreamPipeReaderOptions.UseZeroByteReads) ? 1 : 0) != ((StreamPipeReaderOptions.UseZeroByteReads) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean)
    public static void Method17()
    {
        try { new StreamPipeReaderOptions(null!, 42, 42, true); }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean,System.Boolean)
    public static void Method18()
    {
        try { new StreamPipeReaderOptions(null!, 42, 42, true, true); }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_LeaveOpen:System.Boolean()

    // [20] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_MinimumBufferSize:System.Int32()
    public static void Method20()
    {
        try { if (StreamPipeWriterOptions.MinimumBufferSize != StreamPipeWriterOptions.MinimumBufferSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Method21()
    {
        try { if ((int)(StreamPipeWriterOptions.Pool) != (int)(StreamPipeWriterOptions.Pool)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Boolean)
    public static void Method22()
    {
        try { new StreamPipeWriterOptions(null!, 42, true); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: CustomEntryMethod19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}