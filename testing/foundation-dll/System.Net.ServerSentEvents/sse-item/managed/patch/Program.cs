
public class Program
{
    static int Main()
    {
        int failures = 0;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method0(); failures += SseItemPatchEntry._exitCode << 0;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method1(); failures += SseItemPatchEntry._exitCode << 1;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method2(); failures += SseItemPatchEntry._exitCode << 2;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method3(); failures += SseItemPatchEntry._exitCode << 3;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method4(); failures += SseItemPatchEntry._exitCode << 4;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method5(); failures += SseItemPatchEntry._exitCode << 5;
        SseItemPatchEntry._exitCode = 0; SseItemPatchEntry.Method6(); failures += SseItemPatchEntry._exitCode << 6;
        return failures;
    }
}
