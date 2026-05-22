
public class Program
{
    static int Main()
    {
        int failures = 0;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method0(); failures += ThreadSyncPatchEntry._exitCode << 0;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method1(); failures += ThreadSyncPatchEntry._exitCode << 1;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method2(); failures += ThreadSyncPatchEntry._exitCode << 2;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method3(); failures += ThreadSyncPatchEntry._exitCode << 3;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method4(); failures += ThreadSyncPatchEntry._exitCode << 4;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method5(); failures += ThreadSyncPatchEntry._exitCode << 5;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method6(); failures += ThreadSyncPatchEntry._exitCode << 6;
        ThreadSyncPatchEntry._exitCode = 0; ThreadSyncPatchEntry.Method7(); failures += ThreadSyncPatchEntry._exitCode << 7;
        return failures;
    }
}
