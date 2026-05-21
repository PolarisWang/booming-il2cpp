
public class Program
{
    static int Main()
    {
        int failures = 0;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method0(); failures += SseItemNativeEntry._exitCode << 0;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method1(); failures += SseItemNativeEntry._exitCode << 1;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method2(); failures += SseItemNativeEntry._exitCode << 2;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method3(); failures += SseItemNativeEntry._exitCode << 3;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method4(); failures += SseItemNativeEntry._exitCode << 4;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method5(); failures += SseItemNativeEntry._exitCode << 5;
        SseItemNativeEntry._exitCode = 0; SseItemNativeEntry.Method6(); failures += SseItemNativeEntry._exitCode << 6;
        return failures;
    }
}
