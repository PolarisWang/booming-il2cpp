
public class Program
{
    static int Main()
    {
        int failures = 0;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method0(); failures += JoinsPatchEntry._exitCode << 0;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method1(); failures += JoinsPatchEntry._exitCode << 1;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method2(); failures += JoinsPatchEntry._exitCode << 2;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method3(); failures += JoinsPatchEntry._exitCode << 3;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method4(); failures += JoinsPatchEntry._exitCode << 4;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method5(); failures += JoinsPatchEntry._exitCode << 5;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method6(); failures += JoinsPatchEntry._exitCode << 6;
        JoinsPatchEntry._exitCode = 0; JoinsPatchEntry.Method7(); failures += JoinsPatchEntry._exitCode << 7;
        return failures;
    }
}
