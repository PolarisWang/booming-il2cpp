// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class LoopStateSubjects
{
    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Break:System.Void()
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { default(ParallelLoopState)!.Break(); } catch { }    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsExceptional:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(ParallelLoopState)!.IsExceptional; } catch { }    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsStopped:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = default(ParallelLoopState)!.IsStopped; } catch { }    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_LowestBreakIteration:System.Nullable{System.Int64}()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { _ = default(ParallelLoopState)!.LowestBreakIteration; } catch { }    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_ShouldExitCurrentIteration:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { _ = default(ParallelLoopState)!.ShouldExitCurrentIteration; } catch { }    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Stop:System.Void()
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { default(ParallelLoopState)!.Stop(); } catch { }    }

}
