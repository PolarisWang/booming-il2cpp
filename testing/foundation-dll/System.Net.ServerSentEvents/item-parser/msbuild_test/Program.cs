
public class Program
{
    static int Main()
    {
        int failures = 0;
        ItemParserNativeEntry._exitCode = 0; ItemParserNativeEntry.Method0(); failures += ItemParserNativeEntry._exitCode << 0;
        ItemParserNativeEntry._exitCode = 0; ItemParserNativeEntry.Method1(); failures += ItemParserNativeEntry._exitCode << 1;
        ItemParserNativeEntry._exitCode = 0; ItemParserNativeEntry.Method2(); failures += ItemParserNativeEntry._exitCode << 2;
        ItemParserNativeEntry._exitCode = 0; ItemParserNativeEntry.Method3(); failures += ItemParserNativeEntry._exitCode << 3;
        return failures;
    }
}
