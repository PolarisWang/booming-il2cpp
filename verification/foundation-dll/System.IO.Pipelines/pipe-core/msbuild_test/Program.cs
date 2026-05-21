
public class Program
{
    static int Main()
    {
        int failures = 0;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method0(); failures += PipeCoreNativeEntry._exitCode << 0;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method1(); failures += PipeCoreNativeEntry._exitCode << 1;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method2(); failures += PipeCoreNativeEntry._exitCode << 2;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method3(); failures += PipeCoreNativeEntry._exitCode << 3;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method4(); failures += PipeCoreNativeEntry._exitCode << 4;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method5(); failures += PipeCoreNativeEntry._exitCode << 5;
        PipeCoreNativeEntry._exitCode = 0; PipeCoreNativeEntry.Method6(); failures += PipeCoreNativeEntry._exitCode << 6;
        return failures;
    }
}
