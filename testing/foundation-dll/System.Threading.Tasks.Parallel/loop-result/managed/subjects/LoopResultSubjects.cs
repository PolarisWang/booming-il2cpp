// Auto-generated native-AOT entry point
// Family: family/System.Threading.Tasks.Parallel/loop/result
// Assembly: System.Threading.Tasks.Parallel
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

public static partial class LoopResultSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsCompleted:System.Boolean()
    public static void Subject_0()
    {
        try { if (((default(ParallelLoopResult).IsCompleted) ? 1 : 0) != ((default(ParallelLoopResult).IsCompleted) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_LowestBreakIteration:System.Nullable{System.Int64}()
    public static void Subject_1()
    {
        try { if (((default(ParallelLoopResult).LowestBreakIteration).GetHashCode()) != ((default(ParallelLoopResult).LowestBreakIteration).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}