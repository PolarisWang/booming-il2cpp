
public class Program
{
    static int Main()
    {
        int failures = 0;
        ExtensionsMarshalPatchEntry._exitCode = 0; ExtensionsMarshalPatchEntry.Method0(); failures += ExtensionsMarshalPatchEntry._exitCode << 0;
        ExtensionsMarshalPatchEntry._exitCode = 0; ExtensionsMarshalPatchEntry.Method1(); failures += ExtensionsMarshalPatchEntry._exitCode << 1;
        ExtensionsMarshalPatchEntry._exitCode = 0; ExtensionsMarshalPatchEntry.Method2(); failures += ExtensionsMarshalPatchEntry._exitCode << 2;
        ExtensionsMarshalPatchEntry._exitCode = 0; ExtensionsMarshalPatchEntry.Method3(); failures += ExtensionsMarshalPatchEntry._exitCode << 3;
        return failures;
    }
}
