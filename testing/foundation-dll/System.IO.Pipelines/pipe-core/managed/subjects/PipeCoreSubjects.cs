// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/pipe/core
// Assembly: System.IO.Pipelines
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO.Pipelines;
using System.Linq;

public static partial class PipeCoreSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Input:System.IO.Pipelines.PipeReader()
    public static void Subject_0()
    {
        try { if (((default(IDuplexPipe)!.Input).GetHashCode()) != ((default(IDuplexPipe)!.Input).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Output:System.IO.Pipelines.PipeWriter()
    public static void Subject_1()
    {
        try { if (((default(IDuplexPipe)!.Output).GetHashCode()) != ((default(IDuplexPipe)!.Output).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Reader:System.IO.Pipelines.PipeReader()
    public static void Subject_2()
    {
        try { if (((default(Pipe)!.Reader).GetHashCode()) != ((default(Pipe)!.Reader).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Writer:System.IO.Pipelines.PipeWriter()
    public static void Subject_3()
    {
        try { if (((default(Pipe)!.Writer).GetHashCode()) != ((default(Pipe)!.Writer).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.Pipe::Reset:System.Void()
    public static void Subject_4()
    {
        try { default(Pipe)!.Reset(); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.Pipe::.ctor:System.Void()
    public static void Subject_5()
    {
        try { new Pipe(); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.Pipe::.ctor:System.Void(System.IO.Pipelines.PipeOptions)
    public static void Subject_6()
    {
        try { new Pipe(default); }
        catch { _exitCode = 1; }
    }

}