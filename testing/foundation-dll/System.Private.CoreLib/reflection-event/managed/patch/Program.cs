
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReflectionEventPatchEntry._exitCode = 0; ReflectionEventPatchEntry.Method0(); failures += ReflectionEventPatchEntry._exitCode << 0;
        ReflectionEventPatchEntry._exitCode = 0; ReflectionEventPatchEntry.Method1(); failures += ReflectionEventPatchEntry._exitCode << 1;
        ReflectionEventPatchEntry._exitCode = 0; ReflectionEventPatchEntry.Method2(); failures += ReflectionEventPatchEntry._exitCode << 2;
        return failures;
    }
}
