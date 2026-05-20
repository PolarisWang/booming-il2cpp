
public class Program
{
    static int Main()
    {
        int failures = 0;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod0(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 0;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod1(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 1;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod2(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 2;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod3(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 3;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod4(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 4;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod5(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 5;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod6(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 6;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod7(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 7;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod8(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 8;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod9(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 9;
        ThreadingThreadBasicsPatchEntry._exitCode = 0; ThreadingThreadBasicsPatchEntry.CustomEntryMethod10(); failures += ThreadingThreadBasicsPatchEntry._exitCode << 10;
        return failures;
    }
}
