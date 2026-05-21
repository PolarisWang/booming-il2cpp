
public class Program
{
    static int Main()
    {
        int failures = 0;
        DecoderNativeEntry._exitCode = 0; DecoderNativeEntry.Method0(); failures += DecoderNativeEntry._exitCode << 0;
        DecoderNativeEntry._exitCode = 0; DecoderNativeEntry.Method1(); failures += DecoderNativeEntry._exitCode << 1;
        DecoderNativeEntry._exitCode = 0; DecoderNativeEntry.Method2(); failures += DecoderNativeEntry._exitCode << 2;
        return failures;
    }
}
