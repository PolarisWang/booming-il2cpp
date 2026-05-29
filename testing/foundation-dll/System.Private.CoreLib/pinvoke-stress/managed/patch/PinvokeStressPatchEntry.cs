// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/pinvoke/stress
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class PinvokeStressPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void CustomEntryMethod2()
    {
    }

    // [3] System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void CustomEntryMethod3()
    {
    }

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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}