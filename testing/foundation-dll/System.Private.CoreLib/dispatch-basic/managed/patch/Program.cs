
public class Program
{
    static int Main()
    {
        int failures = 0;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.CustomEntryMethod0(); failures += DispatchBasicPatchEntry._exitCode << 0;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.CustomEntryMethod1(); failures += DispatchBasicPatchEntry._exitCode << 1;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.CustomEntryMethod2(); failures += DispatchBasicPatchEntry._exitCode << 2;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.CustomEntryMethod3(); failures += DispatchBasicPatchEntry._exitCode << 3;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.CustomEntryMethod4(); failures += DispatchBasicPatchEntry._exitCode << 4;
        return failures;
    }
}
