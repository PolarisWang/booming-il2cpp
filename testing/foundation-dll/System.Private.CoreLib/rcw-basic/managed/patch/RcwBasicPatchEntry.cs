// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/rcw/basic
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class RcwBasicPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] RcwBasicNativeEntry/RcwBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void CustomEntryMethod0()
    {
    }

    // [1] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()
    public static void CustomEntryMethod1()
    {
    }

    // [2] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()
    public static void CustomEntryMethod2()
    {
    }

    // [3] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()
    public static void CustomEntryMethod3()
    {
    }

    // [4] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()
    public static void CustomEntryMethod4()
    {
    }

    // [5] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()
    public static void CustomEntryMethod5()
    {
    }

    // [6] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()
    public static void CustomEntryMethod6()
    {
    }

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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}