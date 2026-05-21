// Auto-generated native-AOT entry point
// Family: family/System.Net.ServerSentEvents/item-parser
// Assembly: System.Net.ServerSentEvents
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.ServerSentEvents;

public static partial class ItemParserNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::BeginInvoke:System.IAsyncResult(System.String,System.ReadOnlySpan{System.Byte},System.AsyncCallback,System.Object)
    public static void Method0()
    {
        try { if ((int)(SseItemParser.BeginInvoke("hello", null!, null!, 42)) != (int)(SseItemParser.BeginInvoke("hello", null!, null!, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::EndInvoke:T(System.IAsyncResult)
    public static void Method1()
    {
        try { if ((int)(SseItemParser.EndInvoke(null!)) != (int)(SseItemParser.EndInvoke(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::Invoke:T(System.String,System.ReadOnlySpan{System.Byte})
    public static void Method2()
    {
        try { if ((int)(SseItemParser.Invoke("hello", null!)) != (int)(SseItemParser.Invoke("hello", null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser`1::.ctor:System.Void(System.Object,System.IntPtr)
    public static void Method3()
    {
        try { new SseItemParser(42, IntPtr.Zero); }
        catch { _exitCode = 1; }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}