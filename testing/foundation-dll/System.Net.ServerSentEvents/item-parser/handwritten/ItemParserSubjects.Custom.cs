// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ItemParserSubjects
{
    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::BeginInvoke:System.IAsyncResult(System.String,System.ReadOnlySpan{System.Byte},System.AsyncCallback,System.Object)
    public static void CustomEntrySubject_0()
    {        try { _ = SseItemParser.BeginInvoke("hello", default(ReadOnlySpan<byte>), null!, null!); } catch { _exitCode = 1; }    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::EndInvoke:T(System.IAsyncResult)
    public static void CustomEntrySubject_1()
    {        try { _ = SseItemParser.EndInvoke(null!); } catch { _exitCode = 1; }    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::Invoke:T(System.String,System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_2()
    {        try { _ = SseItemParser.Invoke("hello", default(ReadOnlySpan<byte>)); } catch { _exitCode = 1; }    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::.ctor:System.Void(System.Object,System.IntPtr)
    public static void CustomEntrySubject_3()
    {        try { new SseItemParser<byte>(null!, IntPtr.Zero); } catch { _exitCode = 1; }    }

}
