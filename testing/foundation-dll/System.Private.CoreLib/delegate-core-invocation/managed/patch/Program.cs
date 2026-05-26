
public class Program
{
    static int Main()
    {
        int failures = 0;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.Method0(); failures += DelegateCoreInvocationPatchEntry._exitCode << 0;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.Method1(); failures += DelegateCoreInvocationPatchEntry._exitCode << 1;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.Method2(); failures += DelegateCoreInvocationPatchEntry._exitCode << 2;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.Method3(); failures += DelegateCoreInvocationPatchEntry._exitCode << 3;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.Method4(); failures += DelegateCoreInvocationPatchEntry._exitCode << 4;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.CustomEntryMethod5(); failures += DelegateCoreInvocationPatchEntry._exitCode << 5;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.CustomEntryMethod6(); failures += DelegateCoreInvocationPatchEntry._exitCode << 6;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.Method7(); failures += DelegateCoreInvocationPatchEntry._exitCode << 7;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.CustomEntryMethod8(); failures += DelegateCoreInvocationPatchEntry._exitCode << 8;
        DelegateCoreInvocationPatchEntry._exitCode = 0; DelegateCoreInvocationPatchEntry.CustomEntryMethod9(); failures += DelegateCoreInvocationPatchEntry._exitCode << 9;
        return failures;
    }
}
