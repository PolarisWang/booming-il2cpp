// Auto-generated native-AOT entry point
// Family: family/System.Net.ServerSentEvents/sse-item
// Assembly: System.Net.ServerSentEvents
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.ServerSentEvents;

public static partial class SseItemNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_Data:T()
    public static void Method0()
    {
        try { if ((int)(SseItem.Data) != (int)(SseItem.Data)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventId:System.String()
    public static void Method1()
    {
        try { if (((SseItem.EventId).Length) != ((SseItem.EventId).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_EventType:System.String()
    public static void Method2()
    {
        try { if (((SseItem.EventType).Length) != ((SseItem.EventType).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::get_ReconnectionInterval:System.Nullable{System.TimeSpan}()
    public static void Method3()
    {
        try { if ((int)(SseItem.ReconnectionInterval) != (int)(SseItem.ReconnectionInterval)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_EventId:System.Void(System.String)
    public static void Method4()
    {
        try { SseItem.EventId = "hello"; }
        catch { _exitCode = 1; }
    }

    // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::set_ReconnectionInterval:System.Void(System.Nullable{System.TimeSpan})
    public static void Method5()
    {
        try { SseItem.ReconnectionInterval = null!; }
        catch { _exitCode = 1; }
    }

    // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::.ctor:System.Void(T,System.String)
    public static void Method6()
    {
        // TODO: System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem`1::.ctor:System.Void(T,System.String) could not be auto-generated
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}