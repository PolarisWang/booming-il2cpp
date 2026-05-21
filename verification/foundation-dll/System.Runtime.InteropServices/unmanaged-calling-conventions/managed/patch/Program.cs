
public class Program
{
    static int Main()
    {
        int failures = 0;
        UnmanagedCallingConventionsPatchEntry._exitCode = 0; UnmanagedCallingConventionsPatchEntry.Method0(); failures += UnmanagedCallingConventionsPatchEntry._exitCode << 0;
        return failures;
    }
}
