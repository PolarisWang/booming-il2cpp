
public class Program
{
    static int Main()
    {
        int failures = 0;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method0(); failures += PolymorphismReferencePatchEntry._exitCode << 0;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method1(); failures += PolymorphismReferencePatchEntry._exitCode << 1;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method2(); failures += PolymorphismReferencePatchEntry._exitCode << 2;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method3(); failures += PolymorphismReferencePatchEntry._exitCode << 3;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method4(); failures += PolymorphismReferencePatchEntry._exitCode << 4;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method5(); failures += PolymorphismReferencePatchEntry._exitCode << 5;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method6(); failures += PolymorphismReferencePatchEntry._exitCode << 6;
        PolymorphismReferencePatchEntry._exitCode = 0; PolymorphismReferencePatchEntry.Method7(); failures += PolymorphismReferencePatchEntry._exitCode << 7;
        return failures;
    }
}
