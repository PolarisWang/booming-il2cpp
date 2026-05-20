// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/sync
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ThreadingSyncPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Threading.SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Threading.SemaphoreSlim::Wait:System.Void()

    // [3] System.Private.CoreLib/System.Threading.SemaphoreSlim::Release:System.Int32()

    // [4] System.Private.CoreLib/System.Threading.SemaphoreSlim::get_CurrentCount:System.Int32()

    // [5] System.Private.CoreLib/System.Threading.ManualResetEvent::.ctor:System.Void(System.Boolean)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Threading.ManualResetEvent::Set:System.Boolean()

    // [7] System.Private.CoreLib/System.Threading.ManualResetEvent::Reset:System.Boolean()

    // [8] System.Private.CoreLib/System.Threading.ManualResetEvent::WaitOne:System.Boolean()

    // [9] System.Private.CoreLib/System.Threading.AutoResetEvent::.ctor:System.Void(System.Boolean)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Threading.AutoResetEvent::Set:System.Boolean()

    // [11] System.Private.CoreLib/System.Threading.AutoResetEvent::Reset:System.Boolean()

    // [12] System.Private.CoreLib/System.Threading.AutoResetEvent::WaitOne:System.Boolean()

    // [13] System.Private.CoreLib/System.Threading.SpinLock::.ctor:System.Void(System.Boolean)
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.Private.CoreLib/System.Threading.SpinLock::Enter:System.Void(System.Boolean&)

    // [15] System.Private.CoreLib/System.Threading.SpinLock::Exit:System.Void()

    // [16] System.Private.CoreLib/System.Threading.SpinWait::SpinOnce:System.Void()

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: Subject_5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: Subject_9(); break;
                case 10: CustomEntryMethod10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: CustomEntryMethod12(); break;
                case 13: Subject_13(); break;
                case 14: CustomEntryMethod14(); break;
                case 15: CustomEntryMethod15(); break;
                case 16: CustomEntryMethod16(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}