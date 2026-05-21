
public class Program
{
    static int Main()
    {
        int failures = 0;
        DecoderPatchEntry._exitCode = 0; DecoderPatchEntry.Method0(); failures += DecoderPatchEntry._exitCode << 0;
        DecoderPatchEntry._exitCode = 0; DecoderPatchEntry.Method1(); failures += DecoderPatchEntry._exitCode << 1;
        DecoderPatchEntry._exitCode = 0; DecoderPatchEntry.Method2(); failures += DecoderPatchEntry._exitCode << 2;
        return failures;
    }
}
