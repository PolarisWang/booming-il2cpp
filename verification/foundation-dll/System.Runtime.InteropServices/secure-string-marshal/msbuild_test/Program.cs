
public class Program
{
    static int Main()
    {
        int failures = 0;
        SecureStringMarshalNativeEntry._exitCode = 0; SecureStringMarshalNativeEntry.Method0(); failures += SecureStringMarshalNativeEntry._exitCode << 0;
        SecureStringMarshalNativeEntry._exitCode = 0; SecureStringMarshalNativeEntry.Method1(); failures += SecureStringMarshalNativeEntry._exitCode << 1;
        SecureStringMarshalNativeEntry._exitCode = 0; SecureStringMarshalNativeEntry.Method2(); failures += SecureStringMarshalNativeEntry._exitCode << 2;
        SecureStringMarshalNativeEntry._exitCode = 0; SecureStringMarshalNativeEntry.Method3(); failures += SecureStringMarshalNativeEntry._exitCode << 3;
        return failures;
    }
}
