
public class Program
{
    static int Main()
    {
        int failures = 0;
        OptionsPatchEntry._exitCode = 0; OptionsPatchEntry.Method0(); failures += OptionsPatchEntry._exitCode << 0;
        OptionsPatchEntry._exitCode = 0; OptionsPatchEntry.Method1(); failures += OptionsPatchEntry._exitCode << 1;
        OptionsPatchEntry._exitCode = 0; OptionsPatchEntry.Method2(); failures += OptionsPatchEntry._exitCode << 2;
        return failures;
    }
}
