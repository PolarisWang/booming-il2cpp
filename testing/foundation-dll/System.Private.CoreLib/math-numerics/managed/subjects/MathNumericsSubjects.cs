// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/math/numerics
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class MathNumericsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    public static void Subject_0()
    {
        // non-callable: System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    public static void Subject_1()
    {
        // non-callable: System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    public static void Subject_2()
    {
        // non-callable: System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    public static void Subject_4()
    {
        // non-callable: System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    public static void Subject_5()
    {
        try { _ = (int)(Math.Round(42.0)); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    public static void Subject_6()
    {
        // non-callable: System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    public static void Subject_7()
    {
        // non-callable: System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    public static void Subject_8()
    {
        // non-callable: System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    public static void Subject_9()
    {
        // non-callable: System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    public static void Subject_10()
    {
        // non-callable: System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    public static void Subject_11()
    {
        // non-callable: System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    public static void Subject_12()
    {
        // non-callable: System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    public static void Subject_13()
    {
        // non-callable: System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    public static void Subject_14()
    {
        // non-callable: System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    public static void Subject_15()
    {
        // non-callable: System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    public static void Subject_16()
    {
        try { _ = BitConverter.ToInt32(new byte[4], 0); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    public static void Subject_17()
    {
        try { _ = (int)(BitConverter.ToDouble(new byte[8], 0)); }
        catch { _exitCode = 1; }
    }

}