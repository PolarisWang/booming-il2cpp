
public class Program
{
    static int Main()
    {
        int failures = 0;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod0(); failures += RcwBasicPatchEntry._exitCode << 0;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod1(); failures += RcwBasicPatchEntry._exitCode << 1;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod2(); failures += RcwBasicPatchEntry._exitCode << 2;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod3(); failures += RcwBasicPatchEntry._exitCode << 3;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod4(); failures += RcwBasicPatchEntry._exitCode << 4;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod5(); failures += RcwBasicPatchEntry._exitCode << 5;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.CustomEntryMethod6(); failures += RcwBasicPatchEntry._exitCode << 6;
        return failures;
    }
}
