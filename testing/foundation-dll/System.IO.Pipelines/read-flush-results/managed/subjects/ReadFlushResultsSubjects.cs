// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/read/flush/results
// Assembly: System.IO.Pipelines
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Pipelines;
using System.Linq;

public static partial class ReadFlushResultsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCanceled:System.Boolean()
    public static void Subject_0()
    {
        try { if (((default(FlushResult).IsCanceled) ? 1 : 0) != ((default(FlushResult).IsCanceled) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCompleted:System.Boolean()
    public static void Subject_1()
    {
        try { if (((default(FlushResult).IsCompleted) ? 1 : 0) != ((default(FlushResult).IsCompleted) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.FlushResult::.ctor:System.Void(System.Boolean,System.Boolean)
    public static void Subject_2()
    {
        try { new FlushResult(true, true); }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Buffer:System.Buffers.ReadOnlySequence{System.Byte}()
    public static void Subject_3()
    {
        try { if (((default(ReadResult).Buffer).GetHashCode()) != ((default(ReadResult).Buffer).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCanceled:System.Boolean()
    public static void Subject_4()
    {
        try { if (((default(ReadResult).IsCanceled) ? 1 : 0) != ((default(ReadResult).IsCanceled) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCompleted:System.Boolean()
    public static void Subject_5()
    {
        try { if (((default(ReadResult).IsCompleted) ? 1 : 0) != ((default(ReadResult).IsCompleted) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.ReadResult::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Boolean)
    public static void Subject_6()
    {
        try { new ReadResult(default(ReadOnlySequence<byte>), true, true); }
        catch { _exitCode = 1; }
    }

}