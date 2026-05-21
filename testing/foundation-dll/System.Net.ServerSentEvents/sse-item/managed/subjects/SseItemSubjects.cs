// Auto-generated native-AOT entry point
// Family: family/System.Net.ServerSentEvents/sse/item
// Assembly: System.Net.ServerSentEvents
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.ServerSentEvents;

public static partial class SseItemSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_Data:T()
    public static void Subject_0()
    {
        if (((default(SseItem<byte>).Data).GetHashCode()) != 0) _exitCode = 1;
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventId:System.String()
    public static void Subject_1()
    {
        try { _ = default(SseItem<byte>).EventId; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventType:System.String()
    public static void Subject_2()
    {
        if (((default(SseItem<byte>).EventType).Length) != 7) _exitCode = 1;
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_ReconnectionInterval:System.Nullable{System.TimeSpan}()
    public static void Subject_3()
    {
        if (((default(SseItem<byte>).ReconnectionInterval).GetHashCode()) != 0) _exitCode = 1;
    }

    // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_EventId:System.Void(System.String)
    public static void Subject_4()
    {
        // TODO: System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_EventId:System.Void(System.String) could not be auto-generated
    }

    // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_ReconnectionInterval:System.Void(System.Nullable{System.TimeSpan})
    public static void Subject_5()
    {
        // TODO: System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_ReconnectionInterval:System.Void(System.Nullable{System.TimeSpan}) could not be auto-generated
    }

    // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::.ctor:System.Void(T,System.String)
    public static void Subject_6()
    {
        // TODO: System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::.ctor:System.Void(T,System.String) could not be auto-generated
    }

}