
public class Program
{
    static int Main()
    {
        int failures = 0;
        ItemParserPatchEntry._exitCode = 0; ItemParserPatchEntry.Method0(); failures += ItemParserPatchEntry._exitCode << 0;
        ItemParserPatchEntry._exitCode = 0; ItemParserPatchEntry.Method1(); failures += ItemParserPatchEntry._exitCode << 1;
        ItemParserPatchEntry._exitCode = 0; ItemParserPatchEntry.Method2(); failures += ItemParserPatchEntry._exitCode << 2;
        ItemParserPatchEntry._exitCode = 0; ItemParserPatchEntry.Method3(); failures += ItemParserPatchEntry._exitCode << 3;
        return failures;
    }
}
