
public class Program
{
    static int Main()
    {
        int failures = 0;
        ReflectionHotupdatePatchEntry._exitCode = 0; ReflectionHotupdatePatchEntry.Method0(); failures += ReflectionHotupdatePatchEntry._exitCode << 0;
        ReflectionHotupdatePatchEntry._exitCode = 0; ReflectionHotupdatePatchEntry.Method1(); failures += ReflectionHotupdatePatchEntry._exitCode << 1;
        ReflectionHotupdatePatchEntry._exitCode = 0; ReflectionHotupdatePatchEntry.Method2(); failures += ReflectionHotupdatePatchEntry._exitCode << 2;
        ReflectionHotupdatePatchEntry._exitCode = 0; ReflectionHotupdatePatchEntry.Method3(); failures += ReflectionHotupdatePatchEntry._exitCode << 3;
        return failures;
    }
}
