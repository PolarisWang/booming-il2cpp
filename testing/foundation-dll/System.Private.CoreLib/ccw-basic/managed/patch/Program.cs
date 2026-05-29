
public class Program
{
    static int Main()
    {
        int failures = 0;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.CustomEntryMethod0(); failures += CcwBasicPatchEntry._exitCode << 0;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.CustomEntryMethod1(); failures += CcwBasicPatchEntry._exitCode << 1;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.CustomEntryMethod2(); failures += CcwBasicPatchEntry._exitCode << 2;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.CustomEntryMethod3(); failures += CcwBasicPatchEntry._exitCode << 3;
        CcwBasicPatchEntry._exitCode = 0; CcwBasicPatchEntry.CustomEntryMethod4(); failures += CcwBasicPatchEntry._exitCode << 4;
        return failures;
    }
}
