
public class Program
{
    static int Main()
    {
        int failures = 0;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.Method0(); failures += CcwBasicPatchEntry._exitCode << 0;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.Method1(); failures += CcwBasicPatchEntry._exitCode << 1;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.Method2(); failures += CcwBasicPatchEntry._exitCode << 2;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.Method3(); failures += CcwBasicPatchEntry._exitCode << 3;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.Method4(); failures += CcwBasicPatchEntry._exitCode << 4;
        return failures;
    }
}
