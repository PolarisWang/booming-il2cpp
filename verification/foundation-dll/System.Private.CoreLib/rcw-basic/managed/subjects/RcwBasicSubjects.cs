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
        if (RcwBasicNativeEntry.Run(42) != RcwBasicNativeEntry.Run(42)) _exitCode = 1;
    }

    // [1] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()
    public static void Subject_1()
    {
        if (RcwBasicNativeEntry.TestRcwRoundTripIdentity() != RcwBasicNativeEntry.TestRcwRoundTripIdentity()) _exitCode = 1;
    }

    // [2] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()
    public static void Subject_2()
    {
        if (RcwBasicNativeEntry.TestRcwRoundTripQi() != RcwBasicNativeEntry.TestRcwRoundTripQi()) _exitCode = 1;
    }

    // [3] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()
    public static void Subject_3()
    {
        if (RcwBasicNativeEntry.TestRcwMultipleWrappers() != RcwBasicNativeEntry.TestRcwMultipleWrappers()) _exitCode = 1;
    }

    // [4] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()
    public static void Subject_4()
    {
        if (RcwBasicNativeEntry.TestRcwQiUnknownInterface() != RcwBasicNativeEntry.TestRcwQiUnknownInterface()) _exitCode = 1;
    }

    // [5] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()
    public static void Subject_5()
    {
        if (RcwBasicNativeEntry.TestRcwVtableMethodCall() != RcwBasicNativeEntry.TestRcwVtableMethodCall()) _exitCode = 1;
    }

    // [6] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()
    public static void Subject_6()
    {
        if (RcwBasicNativeEntry.TestRcwDirectVtable() != RcwBasicNativeEntry.TestRcwDirectVtable()) _exitCode = 1;
    }

    public static void Run(int entryIndex)
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
        }
    }

}