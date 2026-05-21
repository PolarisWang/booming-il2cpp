
public class Program
{
    static int Main()
    {
        int failures = 0;
        DataErrorsPatchEntry._exitCode = 0; DataErrorsPatchEntry.Method0(); failures += DataErrorsPatchEntry._exitCode << 0;
        DataErrorsPatchEntry._exitCode = 0; DataErrorsPatchEntry.Method1(); failures += DataErrorsPatchEntry._exitCode << 1;
        DataErrorsPatchEntry._exitCode = 0; DataErrorsPatchEntry.Method2(); failures += DataErrorsPatchEntry._exitCode << 2;
        DataErrorsPatchEntry._exitCode = 0; DataErrorsPatchEntry.Method3(); failures += DataErrorsPatchEntry._exitCode << 3;
        DataErrorsPatchEntry._exitCode = 0; DataErrorsPatchEntry.Method4(); failures += DataErrorsPatchEntry._exitCode << 4;
        DataErrorsPatchEntry._exitCode = 0; DataErrorsPatchEntry.Method5(); failures += DataErrorsPatchEntry._exitCode << 5;
        return failures;
    }
}
