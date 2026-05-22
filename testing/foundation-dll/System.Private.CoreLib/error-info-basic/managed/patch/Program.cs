
public class Program
{
    static int Main()
    {
        int failures = 0;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.Method0(); failures += ErrorInfoBasicPatchEntry._exitCode << 0;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.Method1(); failures += ErrorInfoBasicPatchEntry._exitCode << 1;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.Method2(); failures += ErrorInfoBasicPatchEntry._exitCode << 2;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.Method3(); failures += ErrorInfoBasicPatchEntry._exitCode << 3;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.Method4(); failures += ErrorInfoBasicPatchEntry._exitCode << 4;
        return failures;
    }
}
