
public class Program
{
    static int Main()
    {
        int failures = 0;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method0(); failures += ComWrappersNativeEntry._exitCode << 0;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method1(); failures += ComWrappersNativeEntry._exitCode << 1;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method2(); failures += ComWrappersNativeEntry._exitCode << 2;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method3(); failures += ComWrappersNativeEntry._exitCode << 3;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method4(); failures += ComWrappersNativeEntry._exitCode << 4;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method5(); failures += ComWrappersNativeEntry._exitCode << 5;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method6(); failures += ComWrappersNativeEntry._exitCode << 6;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method7(); failures += ComWrappersNativeEntry._exitCode << 7;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method8(); failures += ComWrappersNativeEntry._exitCode << 8;
        ComWrappersNativeEntry._exitCode = 0; ComWrappersNativeEntry.Method9(); failures += ComWrappersNativeEntry._exitCode << 9;
        return failures;
    }
}
