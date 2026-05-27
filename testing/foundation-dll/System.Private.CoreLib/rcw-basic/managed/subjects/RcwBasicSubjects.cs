// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/rcw/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class RcwBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] RcwBasicNativeEntry/RcwBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { var _ = RcwBasicNativeEntry.Run(42); }
        catch { _exitCode = 1; }
    }

    // [1] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()
    public static void Subject_1()
    {
        try { var _ = RcwBasicNativeEntry.TestRcwRoundTripIdentity(); }
        catch { _exitCode = 1; }
    }

    // [2] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()
    public static void Subject_2()
    {
        try { var _ = RcwBasicNativeEntry.TestRcwRoundTripQi(); }
        catch { _exitCode = 1; }
    }

    // [3] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()
    public static void Subject_3()
    {
        try { var _ = RcwBasicNativeEntry.TestRcwMultipleWrappers(); }
        catch { _exitCode = 1; }
    }

    // [4] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()
    public static void Subject_4()
    {
        try { var _ = RcwBasicNativeEntry.TestRcwQiUnknownInterface(); }
        catch { _exitCode = 1; }
    }

    // [5] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()
    public static void Subject_5()
    {
        try { var _ = RcwBasicNativeEntry.TestRcwVtableMethodCall(); }
        catch { _exitCode = 1; }
    }

    // [6] RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()
    public static void Subject_6()
    {
        try { var _ = RcwBasicNativeEntry.TestRcwDirectVtable(); }
        catch { _exitCode = 1; }
    }

}