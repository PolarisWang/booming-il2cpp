// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/ccw/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class CcwBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] CcwBasicNativeEntry/CcwBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { var _ = CcwBasicNativeEntry.Run(42); }
        catch { _exitCode = 1; }
    }

    // [1] CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()
    public static void Subject_1()
    {
        try { var _ = CcwBasicNativeEntry.TestSimpleMathAdd(); }
        catch { _exitCode = 1; }
    }

    // [2] CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()
    public static void Subject_2()
    {
        try { var _ = CcwBasicNativeEntry.TestSimpleMathMul(); }
        catch { _exitCode = 1; }
    }

    // [3] CcwBasicNativeEntry/CcwBasicNativeEntry::TestConstantValue:System.Int32()
    public static void Subject_3()
    {
        try { var _ = CcwBasicNativeEntry.TestConstantValue(); }
        catch { _exitCode = 1; }
    }

    // [4] CcwBasicNativeEntry/CcwBasicNativeEntry::TestDualInterface:System.Int32()
    public static void Subject_4()
    {
        try { var _ = CcwBasicNativeEntry.TestDualInterface(); }
        catch { _exitCode = 1; }
    }

}