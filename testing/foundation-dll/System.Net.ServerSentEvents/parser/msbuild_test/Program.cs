
public class Program
{
    static int Main()
    {
        int failures = 0;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method0(); failures += ParserNativeEntry._exitCode << 0;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.CustomEntryMethod1(); failures += ParserNativeEntry._exitCode << 1;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method2(); failures += ParserNativeEntry._exitCode << 2;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method3(); failures += ParserNativeEntry._exitCode << 3;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method4(); failures += ParserNativeEntry._exitCode << 4;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method5(); failures += ParserNativeEntry._exitCode << 5;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method6(); failures += ParserNativeEntry._exitCode << 6;
        ParserNativeEntry._exitCode = 0; ParserNativeEntry.Method7(); failures += ParserNativeEntry._exitCode << 7;
        return failures;
    }
}
