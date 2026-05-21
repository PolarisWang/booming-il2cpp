
public class Program
{
    static int Main()
    {
        int failures = 0;
        SchemaPatchEntry._exitCode = 0; SchemaPatchEntry.Method0(); failures += SchemaPatchEntry._exitCode << 0;
        SchemaPatchEntry._exitCode = 0; SchemaPatchEntry.Method1(); failures += SchemaPatchEntry._exitCode << 1;
        return failures;
    }
}
