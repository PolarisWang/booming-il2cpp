// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/loop-result
// Assembly: System.Threading.Tasks.Parallel
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

public static partial class LoopResultNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsCompleted:System.Boolean()
    public static void Method0()
    {
        try { if (((ParallelLoopResult.IsCompleted) ? 1 : 0) != ((ParallelLoopResult.IsCompleted) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_LowestBreakIteration:System.Nullable{System.Int64}()
    public static void Method1()
    {
        try { if ((int)(ParallelLoopResult.LowestBreakIteration) != (int)(ParallelLoopResult.LowestBreakIteration)) _exitCode = 1; }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}