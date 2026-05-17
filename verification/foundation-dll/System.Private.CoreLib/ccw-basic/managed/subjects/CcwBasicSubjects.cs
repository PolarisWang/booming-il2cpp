// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/ccw/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class CcwBasicSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()
    public static void Subject_0()
    {
        if (CcwBasicNativeEntry.TestSimpleMathAdd() != CcwBasicNativeEntry.TestSimpleMathAdd()) _exitCode = 1;
    }

    // [1] CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()
    public static void Subject_1()
    {
        if (CcwBasicNativeEntry.TestSimpleMathMul() != CcwBasicNativeEntry.TestSimpleMathMul()) _exitCode = 1;
    }

    // [2] CcwBasicNativeEntry/CcwBasicNativeEntry::TestConstantValue:System.Int32()
    public static void Subject_2()
    {
        if (CcwBasicNativeEntry.TestConstantValue() != CcwBasicNativeEntry.TestConstantValue()) _exitCode = 1;
    }

    // [3] CcwBasicNativeEntry/CcwBasicNativeEntry::TestDualInterface:System.Int32()
    public static void Subject_3()
    {
        if (CcwBasicNativeEntry.TestDualInterface() != CcwBasicNativeEntry.TestDualInterface()) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
        }
    }

}
