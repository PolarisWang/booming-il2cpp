
public class Program
{
    static int Main()
    {
        int failures = 0;
        GenericsStressPatchEntry._exitCode = 0; GenericsStressPatchEntry.Method0(); failures += GenericsStressPatchEntry._exitCode << 0;
        GenericsStressPatchEntry._exitCode = 0; GenericsStressPatchEntry.Method1(); failures += GenericsStressPatchEntry._exitCode << 1;
        GenericsStressPatchEntry._exitCode = 0; GenericsStressPatchEntry.Method2(); failures += GenericsStressPatchEntry._exitCode << 2;
        return failures;
    }
}
