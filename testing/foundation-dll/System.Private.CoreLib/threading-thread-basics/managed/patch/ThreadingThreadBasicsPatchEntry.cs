// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/threading/thread/basics
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ThreadingThreadBasicsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Threading.Thread::get_ThreadState:System.Threading.ThreadState()
    // [1] System.Private.CoreLib/System.Threading.Thread::get_Priority:System.Threading.ThreadPriority()
    // [2] System.Private.CoreLib/System.Threading.Thread::set_Priority:System.Void(System.Threading.ThreadPriority)
    // [3] System.Private.CoreLib/System.Threading.Thread::get_IsBackground:System.Boolean()
    // [4] System.Private.CoreLib/System.Threading.Thread::set_IsBackground:System.Void(System.Boolean)
    // [5] System.Private.CoreLib/System.Threading.Thread::get_IsThreadPoolThread:System.Boolean()
    // [6] System.Private.CoreLib/System.Threading.Thread::Abort:System.Void()
    // [7] System.Private.CoreLib/System.Threading.Thread::Interrupt:System.Void()
    // [8] System.Private.CoreLib/System.Threading.Thread::ResetAbort:System.Boolean()
    // [9] System.Private.CoreLib/System.Threading.Thread::Yield:System.Boolean()
    // [10] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: CustomEntryMethod9(); break;
                case 10: CustomEntryMethod10(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}