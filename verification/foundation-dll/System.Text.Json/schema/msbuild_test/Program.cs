
public class Program
{
    static int Main()
    {
        int failures = 0;
        SchemaNativeEntry._exitCode = 0; SchemaNativeEntry.Method0(); failures += SchemaNativeEntry._exitCode << 0;
        SchemaNativeEntry._exitCode = 0; SchemaNativeEntry.Method1(); failures += SchemaNativeEntry._exitCode << 1;
        return failures;
    }
}
