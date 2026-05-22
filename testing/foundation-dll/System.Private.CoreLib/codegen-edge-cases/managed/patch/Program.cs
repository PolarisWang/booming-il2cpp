
public class Program
{
    static int Main()
    {
        int failures = 0;
        CodegenEdgeCasesPatchEntry._exitCode = 0; CodegenEdgeCasesPatchEntry.Method0(); failures += CodegenEdgeCasesPatchEntry._exitCode << 0;
        CodegenEdgeCasesPatchEntry._exitCode = 0; CodegenEdgeCasesPatchEntry.Method1(); failures += CodegenEdgeCasesPatchEntry._exitCode << 1;
        CodegenEdgeCasesPatchEntry._exitCode = 0; CodegenEdgeCasesPatchEntry.Method2(); failures += CodegenEdgeCasesPatchEntry._exitCode << 2;
        CodegenEdgeCasesPatchEntry._exitCode = 0; CodegenEdgeCasesPatchEntry.Method3(); failures += CodegenEdgeCasesPatchEntry._exitCode << 3;
        return failures;
    }
}
