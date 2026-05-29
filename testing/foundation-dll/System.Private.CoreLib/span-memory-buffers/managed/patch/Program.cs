
public class Program
{
    static int Main()
    {
        int failures = 0;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method0(); failures += SpanMemoryBuffersPatchEntry._exitCode << 0;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method1(); failures += SpanMemoryBuffersPatchEntry._exitCode << 1;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method2(); failures += SpanMemoryBuffersPatchEntry._exitCode << 2;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method3(); failures += SpanMemoryBuffersPatchEntry._exitCode << 3;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.CustomEntryMethod4(); failures += SpanMemoryBuffersPatchEntry._exitCode << 4;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method5(); failures += SpanMemoryBuffersPatchEntry._exitCode << 5;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method6(); failures += SpanMemoryBuffersPatchEntry._exitCode << 6;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method7(); failures += SpanMemoryBuffersPatchEntry._exitCode << 7;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.CustomEntryMethod8(); failures += SpanMemoryBuffersPatchEntry._exitCode << 8;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method9(); failures += SpanMemoryBuffersPatchEntry._exitCode << 9;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.Method10(); failures += SpanMemoryBuffersPatchEntry._exitCode << 10;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.CustomEntryMethod11(); failures += SpanMemoryBuffersPatchEntry._exitCode << 11;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.CustomEntryMethod12(); failures += SpanMemoryBuffersPatchEntry._exitCode << 12;
        SpanMemoryBuffersPatchEntry._exitCode = 0; SpanMemoryBuffersPatchEntry.CustomEntryMethod13(); failures += SpanMemoryBuffersPatchEntry._exitCode << 13;
        return failures;
    }
}
