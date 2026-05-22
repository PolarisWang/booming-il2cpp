// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/interface/dispatch
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class InterfaceDispatchPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    public static void Run(int entryIndex)
    {
        try
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
        catch
        {
            _exitCode = 1;
        }
    }

}