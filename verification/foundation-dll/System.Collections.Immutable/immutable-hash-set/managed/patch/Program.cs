
public class Program
{
    static int Main()
    {
        int failures = 0;
        ImmutableHashSetPatchEntry._exitCode = 0; ImmutableHashSetPatchEntry.Method0(); failures += ImmutableHashSetPatchEntry._exitCode << 0;
        ImmutableHashSetPatchEntry._exitCode = 0; ImmutableHashSetPatchEntry.Method1(); failures += ImmutableHashSetPatchEntry._exitCode << 1;
        ImmutableHashSetPatchEntry._exitCode = 0; ImmutableHashSetPatchEntry.Method2(); failures += ImmutableHashSetPatchEntry._exitCode << 2;
        ImmutableHashSetPatchEntry._exitCode = 0; ImmutableHashSetPatchEntry.Method3(); failures += ImmutableHashSetPatchEntry._exitCode << 3;
        ImmutableHashSetPatchEntry._exitCode = 0; ImmutableHashSetPatchEntry.Method4(); failures += ImmutableHashSetPatchEntry._exitCode << 4;
        return failures;
    }
}
