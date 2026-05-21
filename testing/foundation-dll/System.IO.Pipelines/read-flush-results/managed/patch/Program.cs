
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method0(); failures += ReadFlushResultsPatchEntry._exitCode << 0;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method1(); failures += ReadFlushResultsPatchEntry._exitCode << 1;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method2(); failures += ReadFlushResultsPatchEntry._exitCode << 2;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method3(); failures += ReadFlushResultsPatchEntry._exitCode << 3;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method4(); failures += ReadFlushResultsPatchEntry._exitCode << 4;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method5(); failures += ReadFlushResultsPatchEntry._exitCode << 5;
        ReadFlushResultsPatchEntry._exitCode = 0; ReadFlushResultsPatchEntry.Method6(); failures += ReadFlushResultsPatchEntry._exitCode << 6;
        return failures;
    }
}
