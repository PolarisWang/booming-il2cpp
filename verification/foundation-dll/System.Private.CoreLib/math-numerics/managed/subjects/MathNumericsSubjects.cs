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
        if (Math.Abs(42) != 42) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    public static void Subject_1()
    {
        if ((int)(Math.Abs(42.0)) != 42) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    public static void Subject_2()
    {
        if ((int)(Math.Ceiling(42.0)) != 42) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    public static void Subject_3()
    {
        if ((int)(Math.Floor(42.0)) != 42) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    public static void Subject_4()
    {
        if ((int)(Math.Round(42.0)) != 42) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    public static void Subject_5()
    {
        if ((int)(Math.Round(42.0)) != 42) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    public static void Subject_6()
    {
        if (Math.Max(42, 42) != 42) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    public static void Subject_7()
    {
        if ((int)(Math.Max(42.0, 42.0)) != 42) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    public static void Subject_8()
    {
        if (Math.Min(42, 42) != 42) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    public static void Subject_9()
    {
        if ((int)(Math.Sqrt(42.0)) != 6) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    public static void Subject_10()
    {
        if ((int)(Math.Pow(42.0, 42.0)) != 2147483647) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    public static void Subject_11()
    {
        if ((int)(Math.Sin(42.0)) != 0) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    public static void Subject_12()
    {
        if ((int)(Math.Cos(42.0)) != 0) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    public static void Subject_13()
    {
        if ((int)(Math.BigMul(42, 42)) != 1764) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    public static void Subject_14()
    {
        if (((BitConverter.GetBytes(42)).Length) != 4) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    public static void Subject_15()
    {
        if (((BitConverter.GetBytes(42.0)).Length) != 8) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    public static void Subject_16()
    {
        if (BitConverter.ToInt32(new byte[4], 0) != 0) _exitCode = 1;
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    public static void Subject_17()
    {
        if ((int)(BitConverter.ToDouble(new byte[8], 0)) != 0) _exitCode = 1;
    }

}