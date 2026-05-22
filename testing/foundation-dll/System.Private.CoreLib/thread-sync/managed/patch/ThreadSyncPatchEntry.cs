// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/thread/sync
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ThreadSyncPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Threading.CancellationTokenSource::Cancel:System.Void()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Threading.CancellationToken::get_IsCancellationRequested:System.Boolean()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Threading.CancellationToken::Register:System.Threading.CancellationTokenRegistration(System.Action)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Threading.ThreadPool::QueueUserWorkItem:System.Threading.WaitCallback(System.Threading.WaitCallback,System.Object)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Threading.WaitCallback::.ctor:System.Void(System.Object,System.IntPtr)
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