// Auto-generated native-AOT entry point
// Family: family/System.Net.ServerSentEvents/formatter
// Assembly: System.Net.ServerSentEvents
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.ServerSentEvents;
using System.Threading;
using System.Threading.Tasks;

public static partial class FormatterNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{System.String}},System.IO.Stream,System.Threading.CancellationToken)
    public static void Method0()
    {
        try { if (((SseFormatter.WriteAsync(null!, null!, null!)).GetHashCode()) != ((SseFormatter.WriteAsync(null!, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::WriteAsync{T}:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}},System.IO.Stream,System.Action{System.Net.ServerSentEvents.SseItem{T},System.Buffers.IBufferWriter{System.Byte}},System.Threading.CancellationToken)

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: CustomEntryMethod1(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}