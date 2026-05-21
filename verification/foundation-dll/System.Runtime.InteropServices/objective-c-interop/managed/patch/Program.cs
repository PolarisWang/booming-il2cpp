
public class Program
{
    static int Main()
    {
        int failures = 0;
        ObjectiveCInteropPatchEntry._exitCode = 0; ObjectiveCInteropPatchEntry.Method0(); failures += ObjectiveCInteropPatchEntry._exitCode << 0;
        ObjectiveCInteropPatchEntry._exitCode = 0; ObjectiveCInteropPatchEntry.Method1(); failures += ObjectiveCInteropPatchEntry._exitCode << 1;
        ObjectiveCInteropPatchEntry._exitCode = 0; ObjectiveCInteropPatchEntry.Method2(); failures += ObjectiveCInteropPatchEntry._exitCode << 2;
        ObjectiveCInteropPatchEntry._exitCode = 0; ObjectiveCInteropPatchEntry.Method3(); failures += ObjectiveCInteropPatchEntry._exitCode << 3;
        return failures;
    }
}
