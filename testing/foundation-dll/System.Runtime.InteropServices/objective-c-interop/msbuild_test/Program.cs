
public class Program
{
    static int Main()
    {
        int failures = 0;
        ObjectiveCInteropNativeEntry._exitCode = 0; ObjectiveCInteropNativeEntry.Method0(); failures += ObjectiveCInteropNativeEntry._exitCode << 0;
        ObjectiveCInteropNativeEntry._exitCode = 0; ObjectiveCInteropNativeEntry.Method1(); failures += ObjectiveCInteropNativeEntry._exitCode << 1;
        ObjectiveCInteropNativeEntry._exitCode = 0; ObjectiveCInteropNativeEntry.Method2(); failures += ObjectiveCInteropNativeEntry._exitCode << 2;
        ObjectiveCInteropNativeEntry._exitCode = 0; ObjectiveCInteropNativeEntry.Method3(); failures += ObjectiveCInteropNativeEntry._exitCode << 3;
        return failures;
    }
}
