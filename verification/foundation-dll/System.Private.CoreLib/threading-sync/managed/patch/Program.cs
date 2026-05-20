
public class Program
{
    static int Main()
    {
        int failures = 0;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.Method0(); failures += ThreadingSyncPatchEntry._exitCode << 0;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.Method1(); failures += ThreadingSyncPatchEntry._exitCode << 1;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod2(); failures += ThreadingSyncPatchEntry._exitCode << 2;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod3(); failures += ThreadingSyncPatchEntry._exitCode << 3;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod4(); failures += ThreadingSyncPatchEntry._exitCode << 4;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.Method5(); failures += ThreadingSyncPatchEntry._exitCode << 5;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod6(); failures += ThreadingSyncPatchEntry._exitCode << 6;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod7(); failures += ThreadingSyncPatchEntry._exitCode << 7;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod8(); failures += ThreadingSyncPatchEntry._exitCode << 8;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.Method9(); failures += ThreadingSyncPatchEntry._exitCode << 9;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod10(); failures += ThreadingSyncPatchEntry._exitCode << 10;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod11(); failures += ThreadingSyncPatchEntry._exitCode << 11;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod12(); failures += ThreadingSyncPatchEntry._exitCode << 12;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.Method13(); failures += ThreadingSyncPatchEntry._exitCode << 13;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod14(); failures += ThreadingSyncPatchEntry._exitCode << 14;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod15(); failures += ThreadingSyncPatchEntry._exitCode << 15;
        ThreadingSyncPatchEntry._exitCode = 0; ThreadingSyncPatchEntry.CustomEntryMethod16(); failures += ThreadingSyncPatchEntry._exitCode << 16;
        return failures;
    }
}
