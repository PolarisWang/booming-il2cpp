
public class Program
{
    static int Main()
    {
        int failures = 0;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.CustomEntryMethod0(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 0;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.CustomEntryMethod1(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 1;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.CustomEntryMethod2(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 2;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method3(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 3;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method4(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 4;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method5(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 5;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method6(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 6;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method7(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 7;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method8(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 8;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method9(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 9;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method10(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 10;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method11(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 11;
        TypeRuntimeHandlesPatchEntry._exitCode = 0; TypeRuntimeHandlesPatchEntry.Method12(); failures += TypeRuntimeHandlesPatchEntry._exitCode << 12;
        return failures;
    }
}
