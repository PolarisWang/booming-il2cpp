
public class Program
{
    static int Main()
    {
        int failures = 0;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.Method0(); failures += DispatchBasicPatchEntry._exitCode << 0;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.Method1(); failures += DispatchBasicPatchEntry._exitCode << 1;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.Method2(); failures += DispatchBasicPatchEntry._exitCode << 2;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.Method3(); failures += DispatchBasicPatchEntry._exitCode << 3;
        DispatchBasicPatchEntry._exitCode = 0; DispatchBasicPatchEntry.Method4(); failures += DispatchBasicPatchEntry._exitCode << 4;
        return failures;
    }
}
