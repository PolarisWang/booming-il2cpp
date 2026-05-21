
public class Program
{
    static int Main()
    {
        int failures = 0;
        LoopResultNativeEntry._exitCode = 0; LoopResultNativeEntry.Method0(); failures += LoopResultNativeEntry._exitCode << 0;
        LoopResultNativeEntry._exitCode = 0; LoopResultNativeEntry.Method1(); failures += LoopResultNativeEntry._exitCode << 1;
        return failures;
    }
}
