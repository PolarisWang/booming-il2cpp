// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class SseItemSubjects
{
    // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_EventId:System.Void(System.String)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try {
            default(SseItem<byte>).EventId = "hello";
        }
        catch { }
    }

    // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_ReconnectionInterval:System.Void(System.Nullable{System.TimeSpan})
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try {
            default(SseItem<byte>).ReconnectionInterval = default;
        }
        catch { }
    }

}
