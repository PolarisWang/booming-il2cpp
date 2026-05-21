
public class Program
{
    static int Main()
    {
        int failures = 0;
        DataErrorsNativeEntry._exitCode = 0; DataErrorsNativeEntry.Method0(); failures += DataErrorsNativeEntry._exitCode << 0;
        DataErrorsNativeEntry._exitCode = 0; DataErrorsNativeEntry.Method1(); failures += DataErrorsNativeEntry._exitCode << 1;
        DataErrorsNativeEntry._exitCode = 0; DataErrorsNativeEntry.Method2(); failures += DataErrorsNativeEntry._exitCode << 2;
        DataErrorsNativeEntry._exitCode = 0; DataErrorsNativeEntry.Method3(); failures += DataErrorsNativeEntry._exitCode << 3;
        DataErrorsNativeEntry._exitCode = 0; DataErrorsNativeEntry.Method4(); failures += DataErrorsNativeEntry._exitCode << 4;
        DataErrorsNativeEntry._exitCode = 0; DataErrorsNativeEntry.Method5(); failures += DataErrorsNativeEntry._exitCode << 5;
        return failures;
    }
}
