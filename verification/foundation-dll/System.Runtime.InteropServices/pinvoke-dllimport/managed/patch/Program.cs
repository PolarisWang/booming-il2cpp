
public class Program
{
    static int Main()
    {
        int failures = 0;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method0(); failures += PinvokeDllimportPatchEntry._exitCode << 0;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method1(); failures += PinvokeDllimportPatchEntry._exitCode << 1;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method2(); failures += PinvokeDllimportPatchEntry._exitCode << 2;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method3(); failures += PinvokeDllimportPatchEntry._exitCode << 3;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method4(); failures += PinvokeDllimportPatchEntry._exitCode << 4;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method5(); failures += PinvokeDllimportPatchEntry._exitCode << 5;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method6(); failures += PinvokeDllimportPatchEntry._exitCode << 6;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method7(); failures += PinvokeDllimportPatchEntry._exitCode << 7;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method8(); failures += PinvokeDllimportPatchEntry._exitCode << 8;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method9(); failures += PinvokeDllimportPatchEntry._exitCode << 9;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method10(); failures += PinvokeDllimportPatchEntry._exitCode << 10;
        PinvokeDllimportPatchEntry._exitCode = 0; PinvokeDllimportPatchEntry.Method11(); failures += PinvokeDllimportPatchEntry._exitCode << 11;
        return failures;
    }
}
