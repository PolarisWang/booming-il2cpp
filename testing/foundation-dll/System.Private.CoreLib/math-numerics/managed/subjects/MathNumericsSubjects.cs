// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/math/numerics
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class MathNumericsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    public static void Subject_0()
    {
        try { _ = Math.Abs(42); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    public static void Subject_1()
    {
        try { _ = (int)(Math.Abs(42.0)); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    public static void Subject_2()
    {
        try { _ = (int)(Math.Ceiling(42.0)); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    public static void Subject_3()
    {
        try { _ = (int)(Math.Floor(42.0)); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    public static void Subject_4()
    {
        try { _ = (int)(Math.Round(42.0)); }
        catch { _exitCode = 1; }
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
        try { _ = Math.Max(42, 42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    public static void Subject_7()
    {
        try { _ = (int)(Math.Max(42.0, 42.0)); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    public static void Subject_8()
    {
        try { _ = Math.Min(42, 42); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    public static void Subject_9()
    {
        try { _ = (int)(Math.Sqrt(42.0)); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    public static void Subject_10()
    {
        try { _ = (int)(Math.Pow(42.0, 42.0)); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    public static void Subject_11()
    {
        try { _ = (int)(Math.Sin(42.0)); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    public static void Subject_12()
    {
        try { _ = (int)(Math.Cos(42.0)); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    public static void Subject_13()
    {
        try { _ = (int)(Math.BigMul(42, 42)); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    public static void Subject_14()
    {
        try { _ = ((BitConverter.GetBytes(42)).Length); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    public static void Subject_15()
    {
        try { _ = ((BitConverter.GetBytes(42.0)).Length); }
        catch { _exitCode = 1; }
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