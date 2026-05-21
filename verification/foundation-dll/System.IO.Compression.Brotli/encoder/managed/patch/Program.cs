
public class Program
{
    static int Main()
    {
        int failures = 0;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method0(); failures += EncoderPatchEntry._exitCode << 0;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method1(); failures += EncoderPatchEntry._exitCode << 1;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method2(); failures += EncoderPatchEntry._exitCode << 2;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method3(); failures += EncoderPatchEntry._exitCode << 3;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method4(); failures += EncoderPatchEntry._exitCode << 4;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method5(); failures += EncoderPatchEntry._exitCode << 5;
        EncoderPatchEntry._exitCode = 0; EncoderPatchEntry.Method6(); failures += EncoderPatchEntry._exitCode << 6;
        return failures;
    }
}
