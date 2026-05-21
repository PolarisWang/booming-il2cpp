// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/dispatch/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using DispatchBasicNativeEntry;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class DispatchBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { if (DispatchBasicNativeEntry.Run(42) != DispatchBasicNativeEntry.Run(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()
    public static void Subject_1()
    {
        try { if (DispatchBasicNativeEntry.TestIDispatchAdd() != DispatchBasicNativeEntry.TestIDispatchAdd()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()
    public static void Subject_2()
    {
        try { if (DispatchBasicNativeEntry.TestIDispatchSub() != DispatchBasicNativeEntry.TestIDispatchSub()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()
    public static void Subject_3()
    {
        try { if (DispatchBasicNativeEntry.TestDualGetStatus() != DispatchBasicNativeEntry.TestDualGetStatus()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()
    public static void Subject_4()
    {
        try { if (DispatchBasicNativeEntry.TestIUnknownGetValue() != DispatchBasicNativeEntry.TestIUnknownGetValue()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}