// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/pipe-core
// Assembly: System.IO.Pipelines
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO.Pipelines;
using System.Linq;

public static partial class PipeCoreNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Input:System.IO.Pipelines.PipeReader()
    public static void Method0()
    {
        try { if ((int)(IDuplexPipe.Input) != (int)(IDuplexPipe.Input)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Output:System.IO.Pipelines.PipeWriter()
    public static void Method1()
    {
        try { if ((int)(IDuplexPipe.Output) != (int)(IDuplexPipe.Output)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Reader:System.IO.Pipelines.PipeReader()
    public static void Method2()
    {
        try { if ((int)(Pipe.Reader) != (int)(Pipe.Reader)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.Pipe::get_Writer:System.IO.Pipelines.PipeWriter()
    public static void Method3()
    {
        try { if ((int)(Pipe.Writer) != (int)(Pipe.Writer)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.Pipe::Reset:System.Void()
    public static void Method4()
    {
        try { Pipe.Reset(); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.Pipe::.ctor:System.Void()
    public static void Method5()
    {
        try { new Pipe(); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.Pipe::.ctor:System.Void(System.IO.Pipelines.PipeOptions)
    public static void Method6()
    {
        try { new Pipe(null!); }
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