
public class Program
{
    static int Main()
    {
        int failures = 0;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method0(); failures += ParserPatchEntry._exitCode << 0;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method1(); failures += ParserPatchEntry._exitCode << 1;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method2(); failures += ParserPatchEntry._exitCode << 2;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method3(); failures += ParserPatchEntry._exitCode << 3;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method4(); failures += ParserPatchEntry._exitCode << 4;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method5(); failures += ParserPatchEntry._exitCode << 5;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method6(); failures += ParserPatchEntry._exitCode << 6;
        ParserPatchEntry._exitCode = 0; ParserPatchEntry.Method7(); failures += ParserPatchEntry._exitCode << 7;
        return failures;
    }
}
