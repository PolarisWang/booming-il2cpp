
public class Program
{
    static int Main()
    {
        int failures = 0;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method0(); failures += FilteringPatchEntry._exitCode << 0;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method1(); failures += FilteringPatchEntry._exitCode << 1;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method2(); failures += FilteringPatchEntry._exitCode << 2;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method3(); failures += FilteringPatchEntry._exitCode << 3;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method4(); failures += FilteringPatchEntry._exitCode << 4;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method5(); failures += FilteringPatchEntry._exitCode << 5;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method6(); failures += FilteringPatchEntry._exitCode << 6;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method7(); failures += FilteringPatchEntry._exitCode << 7;
        FilteringPatchEntry._exitCode = 0; FilteringPatchEntry.Method8(); failures += FilteringPatchEntry._exitCode << 8;
        return failures;
    }
}
