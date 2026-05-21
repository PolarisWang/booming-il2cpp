// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/parallel/options
// Assembly: System.Threading.Tasks.Parallel
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ParallelOptionsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_CancellationToken:System.Threading.CancellationToken()
    public static void Subject_0()
    {
        try { _ = default(ParallelOptions)!.CancellationToken; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_MaxDegreeOfParallelism:System.Int32()
    public static void Subject_1()
    {
        try { _ = default(ParallelOptions)!.MaxDegreeOfParallelism; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_TaskScheduler:System.Threading.Tasks.TaskScheduler()
    public static void Subject_2()
    {
        try { _ = default(ParallelOptions)!.TaskScheduler; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_CancellationToken:System.Void(System.Threading.CancellationToken)
    public static void Subject_3()
    {
        try { _ = default(ParallelOptions)!.CancellationToken = default; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_MaxDegreeOfParallelism:System.Void(System.Int32)
    public static void Subject_4()
    {
        try { _ = default(ParallelOptions)!.MaxDegreeOfParallelism = 42; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_TaskScheduler:System.Void(System.Threading.Tasks.TaskScheduler)
    public static void Subject_5()
    {
        try { _ = default(ParallelOptions)!.TaskScheduler = default; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::.ctor:System.Void()
    public static void Subject_6()
    {
        new ParallelOptions();
    }

}