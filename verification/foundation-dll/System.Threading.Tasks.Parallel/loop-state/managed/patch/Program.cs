
public class Program
{
    static int Main()
    {
        int failures = 0;
        LoopStatePatchEntry._exitCode = 0; LoopStatePatchEntry.Method0(); failures += LoopStatePatchEntry._exitCode << 0;
        LoopStatePatchEntry._exitCode = 0; LoopStatePatchEntry.Method1(); failures += LoopStatePatchEntry._exitCode << 1;
        LoopStatePatchEntry._exitCode = 0; LoopStatePatchEntry.Method2(); failures += LoopStatePatchEntry._exitCode << 2;
        LoopStatePatchEntry._exitCode = 0; LoopStatePatchEntry.Method3(); failures += LoopStatePatchEntry._exitCode << 3;
        LoopStatePatchEntry._exitCode = 0; LoopStatePatchEntry.Method4(); failures += LoopStatePatchEntry._exitCode << 4;
        LoopStatePatchEntry._exitCode = 0; LoopStatePatchEntry.Method5(); failures += LoopStatePatchEntry._exitCode << 5;
        return failures;
    }
}
