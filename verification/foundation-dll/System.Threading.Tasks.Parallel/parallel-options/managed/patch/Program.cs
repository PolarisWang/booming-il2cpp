
public class Program
{
    static int Main()
    {
        int failures = 0;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method0(); failures += ParallelOptionsPatchEntry._exitCode << 0;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method1(); failures += ParallelOptionsPatchEntry._exitCode << 1;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method2(); failures += ParallelOptionsPatchEntry._exitCode << 2;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method3(); failures += ParallelOptionsPatchEntry._exitCode << 3;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method4(); failures += ParallelOptionsPatchEntry._exitCode << 4;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method5(); failures += ParallelOptionsPatchEntry._exitCode << 5;
        ParallelOptionsPatchEntry._exitCode = 0; ParallelOptionsPatchEntry.Method6(); failures += ParallelOptionsPatchEntry._exitCode << 6;
        return failures;
    }
}
