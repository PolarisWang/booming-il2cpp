// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class LoopStateSubjects
{
    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Break:System.Void()
    public static void CustomEntrySubject_0()
    {        try { default(ParallelLoopState)!.Break(); } catch { _exitCode = 1; }    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsExceptional:System.Boolean()
    public static void CustomEntrySubject_1()
    {        try { _ = default(ParallelLoopState)!.IsExceptional; } catch { _exitCode = 1; }    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsStopped:System.Boolean()
    public static void CustomEntrySubject_2()
    {        try { _ = default(ParallelLoopState)!.IsStopped; } catch { _exitCode = 1; }    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_LowestBreakIteration:System.Nullable{System.Int64}()
    public static void CustomEntrySubject_3()
    {        try { _ = default(ParallelLoopState)!.LowestBreakIteration; } catch { _exitCode = 1; }    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_ShouldExitCurrentIteration:System.Boolean()
    public static void CustomEntrySubject_4()
    {        try { _ = default(ParallelLoopState)!.ShouldExitCurrentIteration; } catch { _exitCode = 1; }    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Stop:System.Void()
    public static void CustomEntrySubject_5()
    {        try { default(ParallelLoopState)!.Stop(); } catch { _exitCode = 1; }    }

}
