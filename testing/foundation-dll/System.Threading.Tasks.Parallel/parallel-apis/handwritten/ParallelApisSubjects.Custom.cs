// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ParallelApisSubjects
{
    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action{System.Int32,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_6()
    {
        try {
            Assert.IsNotNull(Parallel.For(42, 42, default, default, default));
        }
        catch { _exitCode = 1; }
    }

    // [7] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int64,System.Int64,System.Threading.Tasks.ParallelOptions,System.Action{System.Int64,System.Threading.Tasks.ParallelLoopState})
    public static void CustomEntrySubject_7()
    {
        try {
            Assert.IsNotNull(Parallel.For(42L, 42L, default, default, default));
        }
        catch { _exitCode = 1; }
    }

    // [42] System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Threading.Tasks.ParallelOptions,System.Action{})
    public static void CustomEntrySubject_42()
    {
        try {
            Parallel.Invoke(default, default);
        }
        catch { _exitCode = 1; }
    }

}
