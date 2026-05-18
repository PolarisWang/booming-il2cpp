// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/ccw/basic
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class CcwBasicPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] CcwBasicNativeEntry/CcwBasicNativeEntry::Run:System.Int32(System.Int32)
    public static int Method0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()
    public static int Method1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()
    public static int Method2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] CcwBasicNativeEntry/CcwBasicNativeEntry::TestConstantValue:System.Int32()
    public static int Method3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] CcwBasicNativeEntry/CcwBasicNativeEntry::TestDualInterface:System.Int32()
    public static int Method4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Method0(); break;
            case 1: Method1(); break;
            case 2: Method2(); break;
            case 3: Method3(); break;
            case 4: Method4(); break;
        }
    }

}