// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/monitor/interlocked
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ThreadingMonitorInterlockedPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)

    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)

    // [2] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)

    // [7] System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)

    // [8] System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)

    // [9] System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)

    // [10] System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)

    // [11] System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)

    // [12] System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: CustomEntryMethod9(); break;
                case 10: CustomEntryMethod10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: Subject_12(); break;
                case 13: Subject_13(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}