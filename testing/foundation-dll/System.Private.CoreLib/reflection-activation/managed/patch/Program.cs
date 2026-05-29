
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.CustomEntryMethod0(); failures += ReflectionActivationPatchEntry._exitCode << 0;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.CustomEntryMethod1(); failures += ReflectionActivationPatchEntry._exitCode << 1;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.CustomEntryMethod2(); failures += ReflectionActivationPatchEntry._exitCode << 2;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.CustomEntryMethod3(); failures += ReflectionActivationPatchEntry._exitCode << 3;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.Method4(); failures += ReflectionActivationPatchEntry._exitCode << 4;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.Method5(); failures += ReflectionActivationPatchEntry._exitCode << 5;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.Method6(); failures += ReflectionActivationPatchEntry._exitCode << 6;
        ReflectionActivationPatchEntry._exitCode = 0; ReflectionActivationPatchEntry.Method7(); failures += ReflectionActivationPatchEntry._exitCode << 7;
        return failures;
    }
}
