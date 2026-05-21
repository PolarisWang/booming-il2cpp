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
        try { _ = default(IDuplexPipe)!.Input; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Output:System.IO.Pipelines.PipeWriter()
    public static void Subject_1()
    {
        try { _ = default(IDuplexPipe)!.Output; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Reader:System.IO.Pipelines.PipeReader()
    public static void Subject_2()
    {
        try { _ = default(Pipe)!.Reader; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Writer:System.IO.Pipelines.PipeWriter()
    public static void Subject_3()
    {
        try { _ = default(Pipe)!.Writer; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.Pipe::Reset:System.Void()
    public static void Subject_4()
    {
        try { default(Pipe)!.Reset(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.Pipe::.ctor:System.Void()
    public static void Subject_5()
    {
        new Pipe();
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.Pipe::.ctor:System.Void(System.IO.Pipelines.PipeOptions)
    public static void Subject_6()
    {
        try { new Pipe(default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

}