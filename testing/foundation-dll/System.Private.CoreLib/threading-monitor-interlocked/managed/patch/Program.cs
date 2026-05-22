
public class Program
{
    static int Main()
    {
        int failures = 0;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod0(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 0;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod1(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 1;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.Method2(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 2;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.Method3(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 3;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod4(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 4;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod5(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 5;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod6(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 6;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod7(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 7;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod8(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 8;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod9(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 9;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod10(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 10;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.CustomEntryMethod11(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 11;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.Method12(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 12;
        ThreadingMonitorInterlockedPatchEntry._exitCode = 0; ThreadingMonitorInterlockedPatchEntry.Method13(); failures += ThreadingMonitorInterlockedPatchEntry._exitCode << 13;
        return failures;
    }
}
