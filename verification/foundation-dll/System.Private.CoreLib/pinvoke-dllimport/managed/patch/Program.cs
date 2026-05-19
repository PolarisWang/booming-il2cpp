
public class Program
{
    static int Main()
    {
        int failures = 0;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method0(); failures += PinvokeDllimportPatchEntry._exitCode << 0;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method1(); failures += PinvokeDllimportPatchEntry._exitCode << 1;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method2(); failures += PinvokeDllimportPatchEntry._exitCode << 2;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method3(); failures += PinvokeDllimportPatchEntry._exitCode << 3;
        return failures;
    }
}
