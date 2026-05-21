// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/loop/state
// Assembly: System.Threading.Tasks.Parallel
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

public static partial class LoopStateSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Break:System.Void()
    public static void Subject_0()
    {
        try { default(ParallelLoopState)!.Break(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsExceptional:System.Boolean()
    public static void Subject_1()
    {
        try { _ = default(ParallelLoopState)!.IsExceptional; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsStopped:System.Boolean()
    public static void Subject_2()
    {
        try { _ = default(ParallelLoopState)!.IsStopped; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_LowestBreakIteration:System.Nullable{System.Int64}()
    public static void Subject_3()
    {
        try { _ = default(ParallelLoopState)!.LowestBreakIteration; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_ShouldExitCurrentIteration:System.Boolean()
    public static void Subject_4()
    {
        try { _ = default(ParallelLoopState)!.ShouldExitCurrentIteration; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Stop:System.Void()
    public static void Subject_5()
    {
        try { default(ParallelLoopState)!.Stop(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

}