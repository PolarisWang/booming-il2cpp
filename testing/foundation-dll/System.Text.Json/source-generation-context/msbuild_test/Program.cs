
public class Program
{
    static int Main()
    {
        int failures = 0;
        SourceGenerationContextNativeEntry._exitCode = 0; SourceGenerationContextNativeEntry.Method0(); failures += SourceGenerationContextNativeEntry._exitCode << 0;
        SourceGenerationContextNativeEntry._exitCode = 0; SourceGenerationContextNativeEntry.Method1(); failures += SourceGenerationContextNativeEntry._exitCode << 1;
        return failures;
    }
}
