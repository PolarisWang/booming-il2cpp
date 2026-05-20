
public class Program
{
    static int Main()
    {
        int failures = 0;
        ImmutableListPatchEntry._exitCode = 0; ImmutableListPatchEntry.Method0(); failures += ImmutableListPatchEntry._exitCode << 0;
        ImmutableListPatchEntry._exitCode = 0; ImmutableListPatchEntry.Method1(); failures += ImmutableListPatchEntry._exitCode << 1;
        ImmutableListPatchEntry._exitCode = 0; ImmutableListPatchEntry.Method2(); failures += ImmutableListPatchEntry._exitCode << 2;
        ImmutableListPatchEntry._exitCode = 0; ImmutableListPatchEntry.Method3(); failures += ImmutableListPatchEntry._exitCode << 3;
        ImmutableListPatchEntry._exitCode = 0; ImmutableListPatchEntry.Method4(); failures += ImmutableListPatchEntry._exitCode << 4;
        ImmutableListPatchEntry._exitCode = 0; ImmutableListPatchEntry.Method5(); failures += ImmutableListPatchEntry._exitCode << 5;
        return failures;
    }
}
