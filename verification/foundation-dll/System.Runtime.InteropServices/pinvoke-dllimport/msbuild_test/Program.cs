
public class Program
{
    static int Main()
    {
        int failures = 0;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method0(); failures += PinvokeDllimportNativeEntry._exitCode << 0;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method1(); failures += PinvokeDllimportNativeEntry._exitCode << 1;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method2(); failures += PinvokeDllimportNativeEntry._exitCode << 2;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method3(); failures += PinvokeDllimportNativeEntry._exitCode << 3;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method4(); failures += PinvokeDllimportNativeEntry._exitCode << 4;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method5(); failures += PinvokeDllimportNativeEntry._exitCode << 5;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method6(); failures += PinvokeDllimportNativeEntry._exitCode << 6;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method7(); failures += PinvokeDllimportNativeEntry._exitCode << 7;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method8(); failures += PinvokeDllimportNativeEntry._exitCode << 8;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method9(); failures += PinvokeDllimportNativeEntry._exitCode << 9;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method10(); failures += PinvokeDllimportNativeEntry._exitCode << 10;
        PinvokeDllimportNativeEntry._exitCode = 0; PinvokeDllimportNativeEntry.Method11(); failures += PinvokeDllimportNativeEntry._exitCode << 11;
        return failures;
    }
}
