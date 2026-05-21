// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/parallel-options
// Assembly: System.Threading.Tasks.Parallel
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ParallelOptionsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_CancellationToken:System.Threading.CancellationToken()
    public static void Method0()
    {
        try { if ((int)(default(ParallelOptions)!.CancellationToken) != (int)(default(ParallelOptions)!.CancellationToken)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_MaxDegreeOfParallelism:System.Int32()
    public static void Method1()
    {
        try { if (default(ParallelOptions)!.MaxDegreeOfParallelism != default(ParallelOptions)!.MaxDegreeOfParallelism) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_TaskScheduler:System.Threading.Tasks.TaskScheduler()
    public static void Method2()
    {
        try { if (((default(ParallelOptions)!.TaskScheduler).GetHashCode()) != ((default(ParallelOptions)!.TaskScheduler).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_CancellationToken:System.Void(System.Threading.CancellationToken)
    public static void Method3()
    {
        try { default(ParallelOptions)!.CancellationToken = default; }
        catch { _exitCode = 1; }
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_MaxDegreeOfParallelism:System.Void(System.Int32)
    public static void Method4()
    {
        try { default(ParallelOptions)!.MaxDegreeOfParallelism = 42; }
        catch { _exitCode = 1; }
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_TaskScheduler:System.Void(System.Threading.Tasks.TaskScheduler)
    public static void Method5()
    {
        try { default(ParallelOptions)!.TaskScheduler = null!; }
        catch { _exitCode = 1; }
    }

    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::.ctor:System.Void()
    public static void Method6()
    {
        try { new ParallelOptions(); }
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