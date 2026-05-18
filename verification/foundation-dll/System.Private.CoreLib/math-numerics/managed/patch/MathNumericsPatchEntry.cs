// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/math/numerics
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class MathNumericsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    public static int Subject_15()
    {
        return unchecked((int)(0xB0000000u + 15));
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    public static int Subject_16()
    {
        return unchecked((int)(0xB0000000u + 16));
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    public static int Subject_17()
    {
        return unchecked((int)(0xB0000000u + 17));
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
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: Subject_12(); break;
                case 13: Subject_13(); break;
                case 14: Subject_14(); break;
                case 15: Subject_15(); break;
                case 16: Subject_16(); break;
                case 17: Subject_17(); break;
            }
        }
        catch (System.Exception)
        {
            _exitCode = 1;
        }
    }

}