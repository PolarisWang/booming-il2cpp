
public class Program
{
    static int Main()
    {
        int failures = 0;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method0(); failures += ParallelOptionsNativeEntry._exitCode << 0;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method1(); failures += ParallelOptionsNativeEntry._exitCode << 1;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method2(); failures += ParallelOptionsNativeEntry._exitCode << 2;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method3(); failures += ParallelOptionsNativeEntry._exitCode << 3;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method4(); failures += ParallelOptionsNativeEntry._exitCode << 4;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method5(); failures += ParallelOptionsNativeEntry._exitCode << 5;
        ParallelOptionsNativeEntry._exitCode = 0; ParallelOptionsNativeEntry.Method6(); failures += ParallelOptionsNativeEntry._exitCode << 6;
        return failures;
    }
}
