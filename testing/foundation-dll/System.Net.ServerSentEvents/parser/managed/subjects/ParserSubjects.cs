// Auto-generated native-AOT entry point
// Family: family/System.Net.ServerSentEvents/parser
// Assembly: System.Net.ServerSentEvents
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.ServerSentEvents;
using System.Threading;

public static partial class ParserSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Create:System.Net.ServerSentEvents.SseParser{System.String}(System.IO.Stream)
    public static void Subject_0()
    {
        if (((SseParser.Create(new MemoryStream())).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Create{T}:System.Net.ServerSentEvents.SseParser{T}(System.IO.Stream,System.Net.ServerSentEvents.SseItemParser{T})
    public static void CustomEntrySubject_1()
    {
    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::Enumerate:System.Collections.Generic.IEnumerable{System.Net.ServerSentEvents.SseItem{T}}()
    public static void Subject_2()
    {
        try { default(SseParser<byte>)!.Enumerate(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::EnumerateAsync:System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}}(System.Threading.CancellationToken)
    public static void Subject_3()
    {
        try { default(SseParser<byte>)!.EnumerateAsync(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::get_LastEventId:System.String()
    public static void Subject_4()
    {
        try { _ = default(SseParser<byte>)!.LastEventId; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::get_ReconnectionInterval:System.TimeSpan()
    public static void Subject_5()
    {
        try { _ = default(SseParser<byte>)!.ReconnectionInterval; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_LastEventId:System.Void(System.String)
    public static void Subject_6()
    {
        // TODO: System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_LastEventId:System.Void(System.String) could not be auto-generated
    }

    // [7] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_ReconnectionInterval:System.Void(System.TimeSpan)
    public static void Subject_7()
    {
        // TODO: System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_ReconnectionInterval:System.Void(System.TimeSpan) could not be auto-generated
    }

}