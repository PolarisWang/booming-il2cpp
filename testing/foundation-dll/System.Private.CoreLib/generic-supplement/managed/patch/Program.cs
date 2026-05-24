
public class Program
{
    static int Main()
    {
        int failures = 0;
        GenericSupplementPatchEntry._exitCode = 0; GenericSupplementPatchEntry.Method0(); failures += GenericSupplementPatchEntry._exitCode << 0;
        GenericSupplementPatchEntry._exitCode = 0; GenericSupplementPatchEntry.Method1(); failures += GenericSupplementPatchEntry._exitCode << 1;
        GenericSupplementPatchEntry._exitCode = 0; GenericSupplementPatchEntry.Method2(); failures += GenericSupplementPatchEntry._exitCode << 2;
        GenericSupplementPatchEntry._exitCode = 0; GenericSupplementPatchEntry.Method3(); failures += GenericSupplementPatchEntry._exitCode << 3;
        return failures;
    }
}
