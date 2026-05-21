// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/formatter
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class FormatterPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{System.String}},System.IO.Stream,System.Threading.CancellationToken)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync{T}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}},System.IO.Stream,System.Action{System.Net.ServerSentEvents.SseItem{T},System.Buffers.IBufferWriter{System.Byte}},System.Threading.CancellationToken)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}