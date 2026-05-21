// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/loop/result
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class LoopResultPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_IsCompleted:System.Boolean()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::get_LowestBreakIteration:System.Nullable{System.Int64}()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}