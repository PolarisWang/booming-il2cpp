
public class Program
{
    static int Main()
    {
        int failures = 0;
        ImmutableArrayPatchEntry._exitCode = 0; ImmutableArrayPatchEntry.Method0(); failures += ImmutableArrayPatchEntry._exitCode << 0;
        ImmutableArrayPatchEntry._exitCode = 0; ImmutableArrayPatchEntry.Method1(); failures += ImmutableArrayPatchEntry._exitCode << 1;
        ImmutableArrayPatchEntry._exitCode = 0; ImmutableArrayPatchEntry.Method2(); failures += ImmutableArrayPatchEntry._exitCode << 2;
        ImmutableArrayPatchEntry._exitCode = 0; ImmutableArrayPatchEntry.Method3(); failures += ImmutableArrayPatchEntry._exitCode << 3;
        ImmutableArrayPatchEntry._exitCode = 0; ImmutableArrayPatchEntry.Method4(); failures += ImmutableArrayPatchEntry._exitCode << 4;
        ImmutableArrayPatchEntry._exitCode = 0; ImmutableArrayPatchEntry.Method5(); failures += ImmutableArrayPatchEntry._exitCode << 5;
        return failures;
    }
}
