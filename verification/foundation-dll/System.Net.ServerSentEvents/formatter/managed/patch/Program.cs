
public class Program
{
    static int Main()
    {
        int failures = 0;
        FormatterPatchEntry._exitCode = 0; FormatterPatchEntry.Method0(); failures += FormatterPatchEntry._exitCode << 0;
        FormatterPatchEntry._exitCode = 0; FormatterPatchEntry.Method1(); failures += FormatterPatchEntry._exitCode << 1;
        return failures;
    }
}
