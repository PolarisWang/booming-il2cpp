
public class Program
{
    static int Main()
    {
        int failures = 0;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method0(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 0;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method1(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 1;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method2(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 2;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method3(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 3;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method4(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 4;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method5(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 5;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method6(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 6;
        WindowsPrincipalRolesNativeEntry._exitCode = 0; WindowsPrincipalRolesNativeEntry.Method7(); failures += WindowsPrincipalRolesNativeEntry._exitCode << 7;
        return failures;
    }
}
