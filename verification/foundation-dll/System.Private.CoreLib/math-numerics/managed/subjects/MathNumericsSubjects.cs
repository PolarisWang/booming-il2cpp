// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/math/numerics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class MathNumericsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    public static void Subject_0()
    {
        if (Math.Abs(42) != Math.Abs(42)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    public static void Subject_1()
    {
        if ((int)(Math.Abs(42.0)) != (int)(Math.Abs(42.0))) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    public static void Subject_2()
    {
        if ((int)(Math.Ceiling(42.0)) != (int)(Math.Ceiling(42.0))) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    public static void Subject_3()
    {
        if ((int)(Math.Floor(42.0)) != (int)(Math.Floor(42.0))) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    public static void Subject_4()
    {
        if ((int)(Math.Round(42.0)) != (int)(Math.Round(42.0))) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    public static void Subject_5()
    {
        if ((int)(Math.Round(42.0)) != (int)(Math.Round(42.0))) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    public static void Subject_6()
    {
        if (Math.Max(42, 42) != Math.Max(42, 42)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    public static void Subject_7()
    {
        if ((int)(Math.Max(42.0, 42.0)) != (int)(Math.Max(42.0, 42.0))) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    public static void Subject_8()
    {
        if (Math.Min(42, 42) != Math.Min(42, 42)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    public static void Subject_9()
    {
        if ((int)(Math.Sqrt(42.0)) != (int)(Math.Sqrt(42.0))) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    public static void Subject_10()
    {
        if ((int)(Math.Pow(42.0, 42.0)) != (int)(Math.Pow(42.0, 42.0))) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    public static void Subject_11()
    {
        if ((int)(Math.Sin(42.0)) != (int)(Math.Sin(42.0))) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    public static void Subject_12()
    {
        if ((int)(Math.Cos(42.0)) != (int)(Math.Cos(42.0))) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    public static void Subject_13()
    {
        if ((int)(Math.BigMul(42, 42)) != (int)(Math.BigMul(42, 42))) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    public static void Subject_14()
    {
        if (((BitConverter.GetBytes(42)).Length) != ((BitConverter.GetBytes(42)).Length)) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    public static void Subject_15()
    {
        if (((BitConverter.GetBytes(42.0)).Length) != ((BitConverter.GetBytes(42.0)).Length)) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    public static void Subject_16()
    {
        // TODO: System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32) could not be auto-generated
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    public static void Subject_17()
    {
        // TODO: System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32) could not be auto-generated
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

}