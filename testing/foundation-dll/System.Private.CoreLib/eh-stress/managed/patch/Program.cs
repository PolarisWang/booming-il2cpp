
public class Program
{
    static int Main()
    {
        int failures = 0;
        EhStressPatchEntry._exitCode = 0; EhStressPatchEntry.Method0(); failures += EhStressPatchEntry._exitCode << 0;
        EhStressPatchEntry._exitCode = 0; EhStressPatchEntry.Method1(); failures += EhStressPatchEntry._exitCode << 1;
        EhStressPatchEntry._exitCode = 0; EhStressPatchEntry.Method2(); failures += EhStressPatchEntry._exitCode << 2;
        EhStressPatchEntry._exitCode = 0; EhStressPatchEntry.Method3(); failures += EhStressPatchEntry._exitCode << 3;
        return failures;
    }
}
