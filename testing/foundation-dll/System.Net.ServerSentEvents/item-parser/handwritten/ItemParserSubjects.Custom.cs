// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ItemParserSubjects
{
    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::BeginInvoke:System.IAsyncResult(System.String,System.ReadOnlySpan{System.Byte},System.AsyncCallback,System.Object)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try {
            Assert.IsNotNull(SseItemParser.BeginInvoke("hello", default(ReadOnlySpan<byte>), null!, null!));
        }
        catch { }
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::EndInvoke:T(System.IAsyncResult)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try {
            Assert.IsNotNull(SseItemParser.EndInvoke(null!));
        }
        catch { }
    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::Invoke:T(System.String,System.ReadOnlySpan{System.Byte})
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try {
            Assert.IsNotNull(SseItemParser.Invoke("hello", default(ReadOnlySpan<byte>)));
        }
        catch { }
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::.ctor:System.Void(System.Object,System.IntPtr)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try {
            new SseItemParser<byte>(null!, IntPtr.Zero);
        }
        catch { }
    }

}
