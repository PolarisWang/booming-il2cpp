
public class Program
{
    static int Main()
    {
        int failures = 0;
        LoopResultPatchEntry._exitCode = 0; LoopResultPatchEntry.Method0(); failures += LoopResultPatchEntry._exitCode << 0;
        LoopResultPatchEntry._exitCode = 0; LoopResultPatchEntry.Method1(); failures += LoopResultPatchEntry._exitCode << 1;
        return failures;
    }
}
