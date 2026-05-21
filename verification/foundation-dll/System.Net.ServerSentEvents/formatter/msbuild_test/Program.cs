
public class Program
{
    static int Main()
    {
        int failures = 0;
        FormatterNativeEntry._exitCode = 0; FormatterNativeEntry.Method0(); failures += FormatterNativeEntry._exitCode << 0;
        FormatterNativeEntry._exitCode = 0; FormatterNativeEntry.CustomEntryMethod1(); failures += FormatterNativeEntry._exitCode << 1;
        return failures;
    }
}
