
public class Program
{
    static int Main()
    {
        int failures = 0;
        TokenAccessPatchEntry._exitCode = 0; TokenAccessPatchEntry.Method0(); failures += TokenAccessPatchEntry._exitCode << 0;
        TokenAccessPatchEntry._exitCode = 0; TokenAccessPatchEntry.Method1(); failures += TokenAccessPatchEntry._exitCode << 1;
        TokenAccessPatchEntry._exitCode = 0; TokenAccessPatchEntry.Method2(); failures += TokenAccessPatchEntry._exitCode << 2;
        TokenAccessPatchEntry._exitCode = 0; TokenAccessPatchEntry.Method3(); failures += TokenAccessPatchEntry._exitCode << 3;
        return failures;
    }
}
