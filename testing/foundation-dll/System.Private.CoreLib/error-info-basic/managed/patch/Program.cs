
public class Program
{
    static int Main()
    {
        int failures = 0;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.CustomEntryMethod0(); failures += ErrorInfoBasicPatchEntry._exitCode << 0;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.CustomEntryMethod1(); failures += ErrorInfoBasicPatchEntry._exitCode << 1;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.CustomEntryMethod2(); failures += ErrorInfoBasicPatchEntry._exitCode << 2;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.CustomEntryMethod3(); failures += ErrorInfoBasicPatchEntry._exitCode << 3;
        ErrorInfoBasicPatchEntry._exitCode = 0; ErrorInfoBasicPatchEntry.CustomEntryMethod4(); failures += ErrorInfoBasicPatchEntry._exitCode << 4;
        return failures;
    }
}
