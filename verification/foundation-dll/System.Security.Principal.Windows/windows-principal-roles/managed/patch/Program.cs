
public class Program
{
    static int Main()
    {
        int failures = 0;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method0(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 0;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method1(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 1;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method2(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 2;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method3(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 3;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method4(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 4;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method5(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 5;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method6(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 6;
        WindowsPrincipalRolesPatchEntry._exitCode = 0; WindowsPrincipalRolesPatchEntry.Method7(); failures += WindowsPrincipalRolesPatchEntry._exitCode << 7;
        return failures;
    }
}
