
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method0(); failures += ReflectionParametersPatchEntry._exitCode << 0;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method1(); failures += ReflectionParametersPatchEntry._exitCode << 1;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method2(); failures += ReflectionParametersPatchEntry._exitCode << 2;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method3(); failures += ReflectionParametersPatchEntry._exitCode << 3;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method4(); failures += ReflectionParametersPatchEntry._exitCode << 4;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method5(); failures += ReflectionParametersPatchEntry._exitCode << 5;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.Method6(); failures += ReflectionParametersPatchEntry._exitCode << 6;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.CustomEntryMethod7(); failures += ReflectionParametersPatchEntry._exitCode << 7;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.CustomEntryMethod8(); failures += ReflectionParametersPatchEntry._exitCode << 8;
        ReflectionParametersPatchEntry._exitCode = 0; ReflectionParametersPatchEntry.CustomEntryMethod9(); failures += ReflectionParametersPatchEntry._exitCode << 9;
        return failures;
    }
}
