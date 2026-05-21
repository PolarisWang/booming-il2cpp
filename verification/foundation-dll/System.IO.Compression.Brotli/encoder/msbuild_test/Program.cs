
public class Program
{
    static int Main()
    {
        int failures = 0;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method0(); failures += EncoderNativeEntry._exitCode << 0;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method1(); failures += EncoderNativeEntry._exitCode << 1;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method2(); failures += EncoderNativeEntry._exitCode << 2;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method3(); failures += EncoderNativeEntry._exitCode << 3;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method4(); failures += EncoderNativeEntry._exitCode << 4;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method5(); failures += EncoderNativeEntry._exitCode << 5;
        EncoderNativeEntry._exitCode = 0; EncoderNativeEntry.Method6(); failures += EncoderNativeEntry._exitCode << 6;
        return failures;
    }
}
