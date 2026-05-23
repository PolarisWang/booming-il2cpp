
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReflectionCustomattrPatchEntry._exitCode = 0; ReflectionCustomattrPatchEntry.Method0(); failures += ReflectionCustomattrPatchEntry._exitCode << 0;
        ReflectionCustomattrPatchEntry._exitCode = 0; ReflectionCustomattrPatchEntry.Method1(); failures += ReflectionCustomattrPatchEntry._exitCode << 1;
        ReflectionCustomattrPatchEntry._exitCode = 0; ReflectionCustomattrPatchEntry.Method2(); failures += ReflectionCustomattrPatchEntry._exitCode << 2;
        return failures;
    }
}
