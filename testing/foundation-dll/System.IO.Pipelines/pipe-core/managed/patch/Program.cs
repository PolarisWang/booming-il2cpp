
public class Program
{
    static int Main()
    {
        int failures = 0;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method0(); failures += PipeCorePatchEntry._exitCode << 0;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method1(); failures += PipeCorePatchEntry._exitCode << 1;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method2(); failures += PipeCorePatchEntry._exitCode << 2;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method3(); failures += PipeCorePatchEntry._exitCode << 3;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method4(); failures += PipeCorePatchEntry._exitCode << 4;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method5(); failures += PipeCorePatchEntry._exitCode << 5;
        PipeCorePatchEntry._exitCode = 0; PipeCorePatchEntry.Method6(); failures += PipeCorePatchEntry._exitCode << 6;
        return failures;
    }
}
