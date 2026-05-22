// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/options/scheduler
// Assembly: System.IO.Pipelines
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Pipelines;
using System.Linq;

public static partial class OptionsSchedulerSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Default:System.IO.Pipelines.PipeOptions()
    public static void Subject_0()
    {
        try { if (((PipeOptions.Default).GetHashCode()) != ((PipeOptions.Default).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_MinimumSegmentSize:System.Int32()
    public static void Subject_1()
    {
        try { if (default(PipeOptions)!.MinimumSegmentSize != default(PipeOptions)!.MinimumSegmentSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_PauseWriterThreshold:System.Int64()
    public static void Subject_2()
    {
        try { if ((int)(default(PipeOptions)!.PauseWriterThreshold) != (int)(default(PipeOptions)!.PauseWriterThreshold)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Subject_3()
    {
        try { if (((default(PipeOptions)!.Pool).GetHashCode()) != ((default(PipeOptions)!.Pool).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ReaderScheduler:System.IO.Pipelines.PipeScheduler()
    public static void Subject_4()
    {
        try { if (((default(PipeOptions)!.ReaderScheduler).GetHashCode()) != ((default(PipeOptions)!.ReaderScheduler).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ResumeWriterThreshold:System.Int64()
    public static void Subject_5()
    {
        try { if ((int)(default(PipeOptions)!.ResumeWriterThreshold) != (int)(default(PipeOptions)!.ResumeWriterThreshold)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_UseSynchronizationContext:System.Boolean()
    public static void Subject_6()
    {
        try { if (((default(PipeOptions)!.UseSynchronizationContext) ? 1 : 0) != ((default(PipeOptions)!.UseSynchronizationContext) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_WriterScheduler:System.IO.Pipelines.PipeScheduler()
    public static void Subject_7()
    {
        try { if (((default(PipeOptions)!.WriterScheduler).GetHashCode()) != ((default(PipeOptions)!.WriterScheduler).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.IO.Pipelines.PipeScheduler,System.IO.Pipelines.PipeScheduler,System.Int64,System.Int64,System.Int32,System.Boolean)
    public static void Subject_8()
    {
        try { new PipeOptions(default, default, default, 42L, 42L, 42, true); }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Inline:System.IO.Pipelines.PipeScheduler()
    public static void Subject_9()
    {
        try { if (((PipeScheduler.Inline).GetHashCode()) != ((PipeScheduler.Inline).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_ThreadPool:System.IO.Pipelines.PipeScheduler()
    public static void Subject_10()
    {
        try { if (((PipeScheduler.ThreadPool).GetHashCode()) != ((PipeScheduler.ThreadPool).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Schedule:System.Void(System.Action{System.Object},System.Object)
    public static void Subject_11()
    {
        try { default(PipeScheduler)!.Schedule(default, 42); }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_BufferSize:System.Int32()
    public static void Subject_12()
    {
        try { if (default(StreamPipeReaderOptions)!.BufferSize != default(StreamPipeReaderOptions)!.BufferSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_LeaveOpen:System.Boolean()
    public static void Subject_13()
    {
        try { if (((default(StreamPipeReaderOptions)!.LeaveOpen) ? 1 : 0) != ((default(StreamPipeReaderOptions)!.LeaveOpen) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_MinimumReadSize:System.Int32()
    public static void Subject_14()
    {
        try { if (default(StreamPipeReaderOptions)!.MinimumReadSize != default(StreamPipeReaderOptions)!.MinimumReadSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Subject_15()
    {
        try { if (((default(StreamPipeReaderOptions)!.Pool).GetHashCode()) != ((default(StreamPipeReaderOptions)!.Pool).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_UseZeroByteReads:System.Boolean()
    public static void Subject_16()
    {
        try { if (((default(StreamPipeReaderOptions)!.UseZeroByteReads) ? 1 : 0) != ((default(StreamPipeReaderOptions)!.UseZeroByteReads) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean)
    public static void Subject_17()
    {
        try { new StreamPipeReaderOptions(default, 42, 42, true); }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean,System.Boolean)
    public static void Subject_18()
    {
        try { new StreamPipeReaderOptions(default, 42, 42, true, true); }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_LeaveOpen:System.Boolean()
    public static void CustomEntrySubject_19()
    {
    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_MinimumBufferSize:System.Int32()
    public static void Subject_20()
    {
        try { if (default(StreamPipeWriterOptions)!.MinimumBufferSize != default(StreamPipeWriterOptions)!.MinimumBufferSize) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Subject_21()
    {
        try { if (((default(StreamPipeWriterOptions)!.Pool).GetHashCode()) != ((default(StreamPipeWriterOptions)!.Pool).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Boolean)
    public static void Subject_22()
    {
        try { new StreamPipeWriterOptions(default, 42, true); }
        catch { _exitCode = 1; }
    }

}