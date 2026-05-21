
public class Program
{
    static int Main()
    {
        int failures = 0;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method0(); failures += NtAccountNativeEntry._exitCode << 0;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method1(); failures += NtAccountNativeEntry._exitCode << 1;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method2(); failures += NtAccountNativeEntry._exitCode << 2;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method3(); failures += NtAccountNativeEntry._exitCode << 3;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method4(); failures += NtAccountNativeEntry._exitCode << 4;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method5(); failures += NtAccountNativeEntry._exitCode << 5;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method6(); failures += NtAccountNativeEntry._exitCode << 6;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method7(); failures += NtAccountNativeEntry._exitCode << 7;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method8(); failures += NtAccountNativeEntry._exitCode << 8;
        NtAccountNativeEntry._exitCode = 0; NtAccountNativeEntry.Method9(); failures += NtAccountNativeEntry._exitCode << 9;
        return failures;
    }
}
