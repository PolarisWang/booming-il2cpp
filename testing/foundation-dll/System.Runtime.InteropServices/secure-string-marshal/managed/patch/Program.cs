
public class Program
{
    static int Main()
    {
        int failures = 0;
        SecureStringMarshalPatchEntry._exitCode = 0; SecureStringMarshalPatchEntry.Method0(); failures += SecureStringMarshalPatchEntry._exitCode << 0;
        SecureStringMarshalPatchEntry._exitCode = 0; SecureStringMarshalPatchEntry.Method1(); failures += SecureStringMarshalPatchEntry._exitCode << 1;
        SecureStringMarshalPatchEntry._exitCode = 0; SecureStringMarshalPatchEntry.Method2(); failures += SecureStringMarshalPatchEntry._exitCode << 2;
        SecureStringMarshalPatchEntry._exitCode = 0; SecureStringMarshalPatchEntry.Method3(); failures += SecureStringMarshalPatchEntry._exitCode << 3;
        return failures;
    }
}
