// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ParserSubjects
{
    // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_LastEventId:System.Void(System.String)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try {
            default(SseParser<byte>)!.LastEventId = "hello";
        }
        catch { }
    }

    // [7] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_ReconnectionInterval:System.Void(System.TimeSpan)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try {
            default(SseParser<byte>)!.ReconnectionInterval = TimeSpan.Zero;
        }
        catch { }
    }

}
