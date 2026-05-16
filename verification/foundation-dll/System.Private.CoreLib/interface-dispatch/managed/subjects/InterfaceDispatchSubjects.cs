// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/interface/dispatch
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class InterfaceDispatchSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        if (InterfaceDispatchNativeEntry.Run(42) != InterfaceDispatchNativeEntry.Run(42)) _exitCode = 1;
    }

    // [1] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()
    public static void Subject_1()
    {
        if (InterfaceDispatchNativeEntry.TestSingleImplSimple() != InterfaceDispatchNativeEntry.TestSingleImplSimple()) _exitCode = 1;
    }

    // [2] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()
    public static void Subject_2()
    {
        if (InterfaceDispatchNativeEntry.TestSingleImplDefault() != InterfaceDispatchNativeEntry.TestSingleImplDefault()) _exitCode = 1;
    }

    // [3] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()
    public static void Subject_3()
    {
        if (InterfaceDispatchNativeEntry.TestMultiImplCalc() != InterfaceDispatchNativeEntry.TestMultiImplCalc()) _exitCode = 1;
    }

    // [4] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()
    public static void Subject_4()
    {
        if (InterfaceDispatchNativeEntry.TestIsCheck() != InterfaceDispatchNativeEntry.TestIsCheck()) _exitCode = 1;
    }

    // [5] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()
    public static void Subject_5()
    {
        if (InterfaceDispatchNativeEntry.TestAsCheck() != InterfaceDispatchNativeEntry.TestAsCheck()) _exitCode = 1;
    }

    // [6] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()
    public static void Subject_6()
    {
        if (InterfaceDispatchNativeEntry.TestDiamondBase() != InterfaceDispatchNativeEntry.TestDiamondBase()) _exitCode = 1;
    }

    // [7] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()
    public static void Subject_7()
    {
        if (InterfaceDispatchNativeEntry.TestDiamondDerived() != InterfaceDispatchNativeEntry.TestDiamondDerived()) _exitCode = 1;
    }

    // [8] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()
    public static void Subject_8()
    {
        if (InterfaceDispatchNativeEntry.TestDiamondMulti() != InterfaceDispatchNativeEntry.TestDiamondMulti()) _exitCode = 1;
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
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
        }
    }

}