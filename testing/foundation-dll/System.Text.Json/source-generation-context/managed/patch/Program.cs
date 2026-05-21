
public class Program
{
    static int Main()
    {
        int failures = 0;
        SourceGenerationContextPatchEntry._exitCode = 0; SourceGenerationContextPatchEntry.Method0(); failures += SourceGenerationContextPatchEntry._exitCode << 0;
        SourceGenerationContextPatchEntry._exitCode = 0; SourceGenerationContextPatchEntry.Method1(); failures += SourceGenerationContextPatchEntry._exitCode << 1;
        return failures;
    }
}
