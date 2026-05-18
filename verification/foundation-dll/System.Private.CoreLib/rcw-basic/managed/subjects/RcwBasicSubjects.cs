// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/rcw/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class RcwBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] RcwBasicNativeEntry/RcwBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { if (RcwBasicNativeEntry.Run(42) != RcwBasicNativeEntry.Run(42)) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

    // [1] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()
    public static void Subject_1()
    {
        try { if (RcwBasicNativeEntry.TestRcwRoundTripIdentity() != RcwBasicNativeEntry.TestRcwRoundTripIdentity()) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

    // [2] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()
    public static void Subject_2()
    {
        try { if (RcwBasicNativeEntry.TestRcwRoundTripQi() != RcwBasicNativeEntry.TestRcwRoundTripQi()) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

    // [3] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()
    public static void Subject_3()
    {
        try { if (RcwBasicNativeEntry.TestRcwMultipleWrappers() != RcwBasicNativeEntry.TestRcwMultipleWrappers()) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

    // [4] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()
    public static void Subject_4()
    {
        try { if (RcwBasicNativeEntry.TestRcwQiUnknownInterface() != RcwBasicNativeEntry.TestRcwQiUnknownInterface()) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

    // [5] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()
    public static void Subject_5()
    {
        try { if (RcwBasicNativeEntry.TestRcwVtableMethodCall() != RcwBasicNativeEntry.TestRcwVtableMethodCall()) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

    // [6] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()
    public static void Subject_6()
    {
        try { if (RcwBasicNativeEntry.TestRcwDirectVtable() != RcwBasicNativeEntry.TestRcwDirectVtable()) _exitCode = 1; }
        catch (Exception) { _exitCode = 1; }
    }

}