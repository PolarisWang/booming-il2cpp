
public class Program
{
    static int Main()
    {
        int failures = 0;
        OptionsNativeEntry._exitCode = 0; OptionsNativeEntry.Method0(); failures += OptionsNativeEntry._exitCode << 0;
        OptionsNativeEntry._exitCode = 0; OptionsNativeEntry.Method1(); failures += OptionsNativeEntry._exitCode << 1;
        OptionsNativeEntry._exitCode = 0; OptionsNativeEntry.Method2(); failures += OptionsNativeEntry._exitCode << 2;
        return failures;
    }
}
