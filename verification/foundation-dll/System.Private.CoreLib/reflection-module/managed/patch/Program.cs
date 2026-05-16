
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReflectionModulePatchEntry._exitCode = 0; ReflectionModulePatchEntry.Method0(); failures += ReflectionModulePatchEntry._exitCode << 0;
        ReflectionModulePatchEntry._exitCode = 0; ReflectionModulePatchEntry.Method1(); failures += ReflectionModulePatchEntry._exitCode << 1;
        ReflectionModulePatchEntry._exitCode = 0; ReflectionModulePatchEntry.Method2(); failures += ReflectionModulePatchEntry._exitCode << 2;
        ReflectionModulePatchEntry._exitCode = 0; ReflectionModulePatchEntry.Method3(); failures += ReflectionModulePatchEntry._exitCode << 3;
        ReflectionModulePatchEntry._exitCode = 0; ReflectionModulePatchEntry.Method4(); failures += ReflectionModulePatchEntry._exitCode << 4;
        ReflectionModulePatchEntry._exitCode = 0; ReflectionModulePatchEntry.Method5(); failures += ReflectionModulePatchEntry._exitCode << 5;
        return failures;
    }
}
