
public class Program
{
    static int Main()
    {
        int failures = 0;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method0(); failures += RcwBasicPatchEntry._exitCode << 0;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method1(); failures += RcwBasicPatchEntry._exitCode << 1;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method2(); failures += RcwBasicPatchEntry._exitCode << 2;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method3(); failures += RcwBasicPatchEntry._exitCode << 3;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method4(); failures += RcwBasicPatchEntry._exitCode << 4;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method5(); failures += RcwBasicPatchEntry._exitCode << 5;
        RcwBasicPatchEntry._exitCode = 0; RcwBasicPatchEntry.Method6(); failures += RcwBasicPatchEntry._exitCode << 6;
        return failures;
    }
}
