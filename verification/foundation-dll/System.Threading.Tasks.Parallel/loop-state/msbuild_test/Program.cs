
public class Program
{
    static int Main()
    {
        int failures = 0;
        LoopStateNativeEntry._exitCode = 0; LoopStateNativeEntry.Method0(); failures += LoopStateNativeEntry._exitCode << 0;
        LoopStateNativeEntry._exitCode = 0; LoopStateNativeEntry.Method1(); failures += LoopStateNativeEntry._exitCode << 1;
        LoopStateNativeEntry._exitCode = 0; LoopStateNativeEntry.Method2(); failures += LoopStateNativeEntry._exitCode << 2;
        LoopStateNativeEntry._exitCode = 0; LoopStateNativeEntry.Method3(); failures += LoopStateNativeEntry._exitCode << 3;
        LoopStateNativeEntry._exitCode = 0; LoopStateNativeEntry.Method4(); failures += LoopStateNativeEntry._exitCode << 4;
        LoopStateNativeEntry._exitCode = 0; LoopStateNativeEntry.Method5(); failures += LoopStateNativeEntry._exitCode << 5;
        return failures;
    }
}
