// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/parser
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ParserPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Create:System.Net.ServerSentEvents.SseParser{System.String}(System.IO.Stream)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Create{T}:System.Net.ServerSentEvents.SseParser{T}(System.IO.Stream,System.Net.ServerSentEvents.SseItemParser{T})
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::Enumerate:System.Collections.Generic.IEnumerable{System.Net.ServerSentEvents.SseItem{T}}()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::EnumerateAsync:System.Collections.Generic.IAsyncEnumerable{System.Net.ServerSentEvents.SseItem{T}}(System.Threading.CancellationToken)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::get_LastEventId:System.String()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::get_ReconnectionInterval:System.TimeSpan()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_LastEventId:System.Void(System.String)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser`1::set_ReconnectionInterval:System.Void(System.TimeSpan)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}