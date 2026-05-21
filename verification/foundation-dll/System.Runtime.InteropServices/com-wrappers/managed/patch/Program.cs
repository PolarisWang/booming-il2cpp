
public class Program
{
    static int Main()
    {
        int failures = 0;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method0(); failures += ComWrappersPatchEntry._exitCode << 0;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method1(); failures += ComWrappersPatchEntry._exitCode << 1;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method2(); failures += ComWrappersPatchEntry._exitCode << 2;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method3(); failures += ComWrappersPatchEntry._exitCode << 3;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method4(); failures += ComWrappersPatchEntry._exitCode << 4;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method5(); failures += ComWrappersPatchEntry._exitCode << 5;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method6(); failures += ComWrappersPatchEntry._exitCode << 6;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method7(); failures += ComWrappersPatchEntry._exitCode << 7;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method8(); failures += ComWrappersPatchEntry._exitCode << 8;
        ComWrappersPatchEntry._exitCode = 0; ComWrappersPatchEntry.Method9(); failures += ComWrappersPatchEntry._exitCode << 9;
        return failures;
    }
}
