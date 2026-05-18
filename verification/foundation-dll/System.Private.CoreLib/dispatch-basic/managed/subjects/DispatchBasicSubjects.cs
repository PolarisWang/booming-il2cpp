// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/dispatch/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class DispatchBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void Subject_0()
    {
        if (DispatchBasicNativeEntry.Run(42) != DispatchBasicNativeEntry.Run(42)) _exitCode = 1;
    }

    // [1] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()
    public static void Subject_1()
    {
        if (DispatchBasicNativeEntry.TestIDispatchAdd() != DispatchBasicNativeEntry.TestIDispatchAdd()) _exitCode = 1;
    }

    // [2] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()
    public static void Subject_2()
    {
        if (DispatchBasicNativeEntry.TestIDispatchSub() != DispatchBasicNativeEntry.TestIDispatchSub()) _exitCode = 1;
    }

    // [3] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()
    public static void Subject_3()
    {
        if (DispatchBasicNativeEntry.TestDualGetStatus() != DispatchBasicNativeEntry.TestDualGetStatus()) _exitCode = 1;
    }

    // [4] DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()
    public static void Subject_4()
    {
        if (DispatchBasicNativeEntry.TestIUnknownGetValue() != DispatchBasicNativeEntry.TestIUnknownGetValue()) _exitCode = 1;
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
        }
    }

}