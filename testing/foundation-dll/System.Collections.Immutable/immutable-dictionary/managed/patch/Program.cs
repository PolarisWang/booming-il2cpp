
public class Program
{
    static int Main()
    {
        int failures = 0;
        ImmutableDictionaryPatchEntry._exitCode = 0; ImmutableDictionaryPatchEntry.Method0(); failures += ImmutableDictionaryPatchEntry._exitCode << 0;
        ImmutableDictionaryPatchEntry._exitCode = 0; ImmutableDictionaryPatchEntry.Method1(); failures += ImmutableDictionaryPatchEntry._exitCode << 1;
        ImmutableDictionaryPatchEntry._exitCode = 0; ImmutableDictionaryPatchEntry.Method2(); failures += ImmutableDictionaryPatchEntry._exitCode << 2;
        return failures;
    }
}
