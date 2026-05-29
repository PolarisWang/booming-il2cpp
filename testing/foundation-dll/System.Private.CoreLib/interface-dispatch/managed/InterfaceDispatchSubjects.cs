// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/interface/dispatch
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class InterfaceDispatchSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { _ = InterfaceDispatchNativeEntry.Run(42); }
        catch { _exitCode = 1; }
    }

    // [1] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()
    public static void Subject_1()
    {
        try { _ = InterfaceDispatchNativeEntry.TestSingleImplSimple(); }
        catch { _exitCode = 1; }
    }

    // [2] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()
    public static void Subject_2()
    {
        try { _ = InterfaceDispatchNativeEntry.TestSingleImplDefault(); }
        catch { _exitCode = 1; }
    }

    // [3] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()
    public static void Subject_3()
    {
        try { _ = InterfaceDispatchNativeEntry.TestMultiImplCalc(); }
        catch { _exitCode = 1; }
    }

    // [4] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()
    public static void Subject_4()
    {
        try { _ = InterfaceDispatchNativeEntry.TestIsCheck(); }
        catch { _exitCode = 1; }
    }

    // [5] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()
    public static void Subject_5()
    {
        try { _ = InterfaceDispatchNativeEntry.TestAsCheck(); }
        catch { _exitCode = 1; }
    }

    // [6] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()
    public static void Subject_6()
    {
        try { _ = InterfaceDispatchNativeEntry.TestDiamondBase(); }
        catch { _exitCode = 1; }
    }

    // [7] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()
    public static void Subject_7()
    {
        try { _ = InterfaceDispatchNativeEntry.TestDiamondDerived(); }
        catch { _exitCode = 1; }
    }

    // [8] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()
    public static void Subject_8()
    {
        try { _ = InterfaceDispatchNativeEntry.TestDiamondMulti(); }
        catch { _exitCode = 1; }
    }

}