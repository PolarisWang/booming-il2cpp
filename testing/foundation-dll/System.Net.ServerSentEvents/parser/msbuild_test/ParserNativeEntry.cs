// Auto-generated native-AOT entry point
// Family: family/System.Net.ServerSentEvents/parser
// Assembly: System.Net.ServerSentEvents
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.ServerSentEvents;
using System.Threading;

public static partial class ParserNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Create:System.Net.ServerSentEvents.SseParser{System.String}(System.IO.Stream)
    public static void Method0()
    {
        try { if ((int)(SseParser.Create(null!)) != (int)(SseParser.Create(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Create{T}:System.Net.ServerSentEvents.SseParser{T}(System.IO.Stream,System.Net.ServerSentEvents.SseItemParser{T})

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::Enumerate:System.Collections.Generic.IEnumerable{System.Net.ServerSentEvents.SseItem{T}}()
    public static void Method2()
    {
        try { if ((int)(SseParser.Enumerate()) != (int)(SseParser.Enumerate())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::EnumerateAsync:System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}}(System.Threading.CancellationToken)
    public static void Method3()
    {
        try { if ((int)(SseParser.EnumerateAsync(null!)) != (int)(SseParser.EnumerateAsync(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::get_LastEventId:System.String()
    public static void Method4()
    {
        try { if (((SseParser.LastEventId).Length) != ((SseParser.LastEventId).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::get_ReconnectionInterval:System.TimeSpan()
    public static void Method5()
    {
        try { if (((SseParser.ReconnectionInterval).GetHashCode()) != ((SseParser.ReconnectionInterval).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_LastEventId:System.Void(System.String)
    public static void Method6()
    {
        try { SseParser.LastEventId = "hello"; }
        catch { _exitCode = 1; }
    }

    // [7] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_ReconnectionInterval:System.Void(System.TimeSpan)
    public static void Method7()
    {
        try { SseParser.ReconnectionInterval = TimeSpan.Zero; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}