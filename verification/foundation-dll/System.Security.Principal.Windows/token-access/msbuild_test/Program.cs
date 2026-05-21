
public class Program
{
    static int Main()
    {
        int failures = 0;
        TokenAccessNativeEntry._exitCode = 0; TokenAccessNativeEntry.Method0(); failures += TokenAccessNativeEntry._exitCode << 0;
        TokenAccessNativeEntry._exitCode = 0; TokenAccessNativeEntry.Method1(); failures += TokenAccessNativeEntry._exitCode << 1;
        TokenAccessNativeEntry._exitCode = 0; TokenAccessNativeEntry.Method2(); failures += TokenAccessNativeEntry._exitCode << 2;
        TokenAccessNativeEntry._exitCode = 0; TokenAccessNativeEntry.Method3(); failures += TokenAccessNativeEntry._exitCode << 3;
        return failures;
    }
}
