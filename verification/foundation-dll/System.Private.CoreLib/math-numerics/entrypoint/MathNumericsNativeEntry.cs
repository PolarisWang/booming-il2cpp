// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/math/numerics
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class MathNumericsNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    static int Method0()
    {
        return Math.Abs(42);
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    static int Method1()
    {
        return (int)(Math.Abs(42.0));
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    static int Method2()
    {
        return (int)(Math.Ceiling(42.0));
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    static int Method3()
    {
        return (int)(Math.Floor(42.0));
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    static int Method4()
    {
        return (int)(Math.Round(42.0));
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    static int Method5()
    {
        return (int)(Math.Round(42.0));
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    static int Method6()
    {
        return Math.Max(42, 42);
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    static int Method7()
    {
        return (int)(Math.Max(42.0, 42.0));
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    static int Method8()
    {
        return Math.Min(42, 42);
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    static int Method9()
    {
        return (int)(Math.Sqrt(42.0));
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    static int Method10()
    {
        return (int)(Math.Pow(42.0, 42.0));
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    static int Method11()
    {
        return (int)(Math.Sin(42.0));
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    static int Method12()
    {
        return (int)(Math.Cos(42.0));
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    static int Method13()
    {
        return (int)(Math.BigMul(42, 42));
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    static int Method14()
    {
        return ((BitConverter.GetBytes(42)).Length);
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    static int Method15()
    {
        return ((BitConverter.GetBytes(42.0)).Length);
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    static int Method16()
    {
        // TODO: System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32) could not be auto-generated
        return 0;
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    static int Method17()
    {
        // TODO: System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32) could not be auto-generated
        return 0;
    }

}