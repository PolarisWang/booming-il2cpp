// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/math/numerics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class MathNumericsSubjects
{
    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    public static int Subject_0()
    {
        return Math.Abs(42);
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    public static int Subject_1()
    {
        return (int)(Math.Abs(42.0));
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    public static int Subject_2()
    {
        return (int)(Math.Ceiling(42.0));
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    public static int Subject_3()
    {
        return (int)(Math.Floor(42.0));
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    public static int Subject_4()
    {
        return (int)(Math.Round(42.0));
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    public static int Subject_5()
    {
        return (int)(Math.Round(42.0));
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    public static int Subject_6()
    {
        return Math.Max(42, 42);
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    public static int Subject_7()
    {
        return (int)(Math.Max(42.0, 42.0));
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    public static int Subject_8()
    {
        return Math.Min(42, 42);
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    public static int Subject_9()
    {
        return (int)(Math.Sqrt(42.0));
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    public static int Subject_10()
    {
        return (int)(Math.Pow(42.0, 42.0));
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    public static int Subject_11()
    {
        return (int)(Math.Sin(42.0));
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    public static int Subject_12()
    {
        return (int)(Math.Cos(42.0));
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    public static int Subject_13()
    {
        return (int)(Math.BigMul(42, 42));
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    public static int Subject_14()
    {
        return ((BitConverter.GetBytes(42)).Length);
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    public static int Subject_15()
    {
        return ((BitConverter.GetBytes(42.0)).Length);
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    public static int Subject_16()
    {
        return BitConverter.ToInt32(new byte[4], 0);
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    public static int Subject_17()
    {
        return (int)(BitConverter.ToDouble(new byte[8], 0));
    }

}