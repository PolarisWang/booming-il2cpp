
public class Program
{
    static int Main()
    {
        int failures = 0;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method0(); failures += OptionsErrorsNativeEntry._exitCode << 0;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method1(); failures += OptionsErrorsNativeEntry._exitCode << 1;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method2(); failures += OptionsErrorsNativeEntry._exitCode << 2;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method3(); failures += OptionsErrorsNativeEntry._exitCode << 3;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method4(); failures += OptionsErrorsNativeEntry._exitCode << 4;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method5(); failures += OptionsErrorsNativeEntry._exitCode << 5;
        OptionsErrorsNativeEntry._exitCode = 0; OptionsErrorsNativeEntry.Method6(); failures += OptionsErrorsNativeEntry._exitCode << 6;
        return failures;
    }
}
