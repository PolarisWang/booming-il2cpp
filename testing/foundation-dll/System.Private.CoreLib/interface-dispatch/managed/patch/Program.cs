
public class Program
{
    static int Main()
    {
        int failures = 0;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method0(); failures += InterfaceDispatchPatchEntry._exitCode << 0;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method1(); failures += InterfaceDispatchPatchEntry._exitCode << 1;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method2(); failures += InterfaceDispatchPatchEntry._exitCode << 2;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method3(); failures += InterfaceDispatchPatchEntry._exitCode << 3;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method4(); failures += InterfaceDispatchPatchEntry._exitCode << 4;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method5(); failures += InterfaceDispatchPatchEntry._exitCode << 5;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method6(); failures += InterfaceDispatchPatchEntry._exitCode << 6;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method7(); failures += InterfaceDispatchPatchEntry._exitCode << 7;
        InterfaceDispatchPatchEntry._exitCode = 0; InterfaceDispatchPatchEntry.Method8(); failures += InterfaceDispatchPatchEntry._exitCode << 8;
        return failures;
    }
}
