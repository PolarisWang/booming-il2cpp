
public class Program
{
    static int Main()
    {
        int failures = 0;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method0(); failures += OptionsErrorsPatchEntry._exitCode << 0;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method1(); failures += OptionsErrorsPatchEntry._exitCode << 1;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method2(); failures += OptionsErrorsPatchEntry._exitCode << 2;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method3(); failures += OptionsErrorsPatchEntry._exitCode << 3;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method4(); failures += OptionsErrorsPatchEntry._exitCode << 4;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method5(); failures += OptionsErrorsPatchEntry._exitCode << 5;
        OptionsErrorsPatchEntry._exitCode = 0; OptionsErrorsPatchEntry.Method6(); failures += OptionsErrorsPatchEntry._exitCode << 6;
        return failures;
    }
}
