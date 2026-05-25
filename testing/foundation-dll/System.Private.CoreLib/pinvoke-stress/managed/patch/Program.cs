
public class Program
{
    static int Main()
    {
        int failures = 0;
        PinvokeStressPatchEntry._exitCode = 0; PinvokeStressPatchEntry.Method0(); failures += PinvokeStressPatchEntry._exitCode << 0;
        PinvokeStressPatchEntry._exitCode = 0; PinvokeStressPatchEntry.Method1(); failures += PinvokeStressPatchEntry._exitCode << 1;
        PinvokeStressPatchEntry._exitCode = 0; PinvokeStressPatchEntry.Method2(); failures += PinvokeStressPatchEntry._exitCode << 2;
        PinvokeStressPatchEntry._exitCode = 0; PinvokeStressPatchEntry.Method3(); failures += PinvokeStressPatchEntry._exitCode << 3;
        return failures;
    }
}
