
public class Program
{
    static int Main()
    {
        int failures = 0;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.CustomEntryMethod0(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 0;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.CustomEntryMethod1(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 1;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.Method2(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 2;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.CustomEntryMethod3(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 3;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.CustomEntryMethod4(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 4;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.CustomEntryMethod5(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 5;
        ObjectEqualityIdentityPatchEntry._exitCode = 0; ObjectEqualityIdentityPatchEntry.CustomEntryMethod6(); failures += ObjectEqualityIdentityPatchEntry._exitCode << 6;
        return failures;
    }
}
