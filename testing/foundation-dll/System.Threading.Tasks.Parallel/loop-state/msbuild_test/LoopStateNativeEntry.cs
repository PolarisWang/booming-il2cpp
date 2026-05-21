// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/loop-state
// Assembly: System.Threading.Tasks.Parallel
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

public static partial class LoopStateNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Break:System.Void()
    public static void Method0()
    {
        try { ParallelLoopState.Break(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsExceptional:System.Boolean()
    public static void Method1()
    {
        try { if (((ParallelLoopState.IsExceptional) ? 1 : 0) != ((ParallelLoopState.IsExceptional) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_IsStopped:System.Boolean()
    public static void Method2()
    {
        try { if (((ParallelLoopState.IsStopped) ? 1 : 0) != ((ParallelLoopState.IsStopped) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_LowestBreakIteration:System.Nullable{System.Int64}()
    public static void Method3()
    {
        try { if ((int)(ParallelLoopState.LowestBreakIteration) != (int)(ParallelLoopState.LowestBreakIteration)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::get_ShouldExitCurrentIteration:System.Boolean()
    public static void Method4()
    {
        try { if (((ParallelLoopState.ShouldExitCurrentIteration) ? 1 : 0) != ((ParallelLoopState.ShouldExitCurrentIteration) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopState::Stop:System.Void()
    public static void Method5()
    {
        try { ParallelLoopState.Stop(); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}