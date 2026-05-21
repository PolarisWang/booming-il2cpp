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
        if (((PipeOptions.Default).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_MinimumSegmentSize:System.Int32()
    public static void Subject_1()
    {
        try { _ = default(PipeOptions)!.MinimumSegmentSize; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_PauseWriterThreshold:System.Int64()
    public static void Subject_2()
    {
        try { _ = default(PipeOptions)!.PauseWriterThreshold; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Subject_3()
    {
        try { _ = default(PipeOptions)!.Pool; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ReaderScheduler:System.IO.Pipelines.PipeScheduler()
    public static void Subject_4()
    {
        try { _ = default(PipeOptions)!.ReaderScheduler; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_ResumeWriterThreshold:System.Int64()
    public static void Subject_5()
    {
        try { _ = default(PipeOptions)!.ResumeWriterThreshold; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_UseSynchronizationContext:System.Boolean()
    public static void Subject_6()
    {
        try { _ = default(PipeOptions)!.UseSynchronizationContext; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::get_WriterScheduler:System.IO.Pipelines.PipeScheduler()
    public static void Subject_7()
    {
        try { _ = default(PipeOptions)!.WriterScheduler; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.IO.Pipelines.PipeScheduler,System.IO.Pipelines.PipeScheduler,System.Int64,System.Int64,System.Int32,System.Boolean)
    public static void Subject_8()
    {
        new PipeOptions(default, default, default, 42L, 42L, 42, true);
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_Inline:System.IO.Pipelines.PipeScheduler()
    public static void Subject_9()
    {
        if (((PipeScheduler.Inline).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::get_ThreadPool:System.IO.Pipelines.PipeScheduler()
    public static void Subject_10()
    {
        if (((PipeScheduler.ThreadPool).GetHashCode()) != 115000) _exitCode = 1;
    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeScheduler::Schedule:System.Void(System.Action{System.Object},System.Object)
    public static void Subject_11()
    {
        try { default(PipeScheduler)!.Schedule(default, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_BufferSize:System.Int32()
    public static void Subject_12()
    {
        try { _ = default(StreamPipeReaderOptions)!.BufferSize; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_LeaveOpen:System.Boolean()
    public static void Subject_13()
    {
        try { _ = default(StreamPipeReaderOptions)!.LeaveOpen; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_MinimumReadSize:System.Int32()
    public static void Subject_14()
    {
        try { _ = default(StreamPipeReaderOptions)!.MinimumReadSize; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Subject_15()
    {
        try { _ = default(StreamPipeReaderOptions)!.Pool; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::get_UseZeroByteReads:System.Boolean()
    public static void Subject_16()
    {
        try { _ = default(StreamPipeReaderOptions)!.UseZeroByteReads; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean)
    public static void Subject_17()
    {
        new StreamPipeReaderOptions(default, 42, 42, true);
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.StreamPipeReaderOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Int32,System.Boolean,System.Boolean)
    public static void Subject_18()
    {
        new StreamPipeReaderOptions(default, 42, 42, true, true);
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_LeaveOpen:System.Boolean()
    public static void CustomEntrySubject_19()
    {
    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_MinimumBufferSize:System.Int32()
    public static void Subject_20()
    {
        try { _ = default(StreamPipeWriterOptions)!.MinimumBufferSize; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::get_Pool:System.Buffers.MemoryPool{System.Byte}()
    public static void Subject_21()
    {
        try { _ = default(StreamPipeWriterOptions)!.Pool; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.StreamPipeWriterOptions::.ctor:System.Void(System.Buffers.MemoryPool{System.Byte},System.Int32,System.Boolean)
    public static void Subject_22()
    {
        new StreamPipeWriterOptions(default, 42, true);
    }

}