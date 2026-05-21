// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/read-flush-results
// Assembly: System.IO.Pipelines
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO.Pipelines;
using System.Linq;

public static partial class ReadFlushResultsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCanceled:System.Boolean()
    public static void Method0()
    {
        try { if (((FlushResult.IsCanceled) ? 1 : 0) != ((FlushResult.IsCanceled) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsCompleted:System.Boolean()
    public static void Method1()
    {
        try { if (((FlushResult.IsCompleted) ? 1 : 0) != ((FlushResult.IsCompleted) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.FlushResult::.ctor:System.Void(System.Boolean,System.Boolean)
    public static void Method2()
    {
        try { new FlushResult(true, true); }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Buffer:System.Buffers.ReadOnlySequence{System.Byte}()
    public static void Method3()
    {
        try { if ((int)(ReadResult.Buffer) != (int)(ReadResult.Buffer)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCanceled:System.Boolean()
    public static void Method4()
    {
        try { if (((ReadResult.IsCanceled) ? 1 : 0) != ((ReadResult.IsCanceled) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsCompleted:System.Boolean()
    public static void Method5()
    {
        try { if (((ReadResult.IsCompleted) ? 1 : 0) != ((ReadResult.IsCompleted) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.ReadResult::.ctor:System.Void(System.Buffers.ReadOnlySequence{System.Byte},System.Boolean,System.Boolean)
    public static void Method6()
    {
        try { new ReadResult(null!, true, true); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}