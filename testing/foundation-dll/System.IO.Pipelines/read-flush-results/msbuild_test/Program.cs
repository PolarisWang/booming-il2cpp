
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method0(); failures += ReadFlushResultsNativeEntry._exitCode << 0;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method1(); failures += ReadFlushResultsNativeEntry._exitCode << 1;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method2(); failures += ReadFlushResultsNativeEntry._exitCode << 2;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method3(); failures += ReadFlushResultsNativeEntry._exitCode << 3;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method4(); failures += ReadFlushResultsNativeEntry._exitCode << 4;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method5(); failures += ReadFlushResultsNativeEntry._exitCode << 5;
        ReadFlushResultsNativeEntry._exitCode = 0; ReadFlushResultsNativeEntry.Method6(); failures += ReadFlushResultsNativeEntry._exitCode << 6;
        return failures;
    }
}
