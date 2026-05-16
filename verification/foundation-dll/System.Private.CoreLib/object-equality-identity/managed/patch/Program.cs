
public class Program
{
    static int Main()
    {
        int failures = 0;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method0(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 0;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method1(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 1;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method2(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 2;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method3(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 3;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method4(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 4;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method5(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 5;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method6(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 6;
        return failures;
    }
}
