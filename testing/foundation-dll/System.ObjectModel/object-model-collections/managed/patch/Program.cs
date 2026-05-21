
public class Program
{
    static int Main()
    {
        int failures = 0;
        ObjectModelCollectionsPatchEntry._exitCode = 0; ObjectModelCollectionsPatchEntry.Method0(); failures += ObjectModelCollectionsPatchEntry._exitCode << 0;
        ObjectModelCollectionsPatchEntry._exitCode = 0; ObjectModelCollectionsPatchEntry.Method1(); failures += ObjectModelCollectionsPatchEntry._exitCode << 1;
        ObjectModelCollectionsPatchEntry._exitCode = 0; ObjectModelCollectionsPatchEntry.Method2(); failures += ObjectModelCollectionsPatchEntry._exitCode << 2;
        ObjectModelCollectionsPatchEntry._exitCode = 0; ObjectModelCollectionsPatchEntry.Method3(); failures += ObjectModelCollectionsPatchEntry._exitCode << 3;
        ObjectModelCollectionsPatchEntry._exitCode = 0; ObjectModelCollectionsPatchEntry.Method4(); failures += ObjectModelCollectionsPatchEntry._exitCode << 4;
        ObjectModelCollectionsPatchEntry._exitCode = 0; ObjectModelCollectionsPatchEntry.Method5(); failures += ObjectModelCollectionsPatchEntry._exitCode << 5;
        return failures;
    }
}
