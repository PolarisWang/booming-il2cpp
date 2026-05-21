// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/enum/parsing
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class EnumParsingSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
    public static void Subject_0()
    {
        try { if (((Enum.Format(typeof(byte), 42, "hello")).Length) != ((Enum.Format(typeof(byte), 42, "hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    public static void Subject_1()
    {
        try { if (((Enum.GetName(typeof(DayOfWeek), (object)1)).Length) != ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    public static void Subject_2()
    {
        try { if (((Enum.GetName(typeof(DayOfWeek), (object)1)).Length) != ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    public static void Subject_3()
    {
        try { if (((Enum.GetNames(typeof(byte))).Length) != ((Enum.GetNames(typeof(byte))).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    public static void Subject_4()
    {
        try { if (((Enum.GetValues(typeof(byte))).GetHashCode()) != ((Enum.GetValues(typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    public static void Subject_5()
    {
        try { if (((Enum.IsDefined(typeof(byte), 42)) ? 1 : 0) != ((Enum.IsDefined(typeof(byte), 42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    public static void Subject_6()
    {
        try { if (((Enum.Parse(typeof(byte), "hello")).GetHashCode()) != ((Enum.Parse(typeof(byte), "hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    public static void Subject_7()
    {
        try { if (((Enum.Parse(typeof(byte), "hello", true)).GetHashCode()) != ((Enum.Parse(typeof(byte), "hello", true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    public static void Subject_8()
    {
        try { if (((DayOfWeek.Monday.ToString()).Length) != ((DayOfWeek.Monday.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    public static void Subject_9()
    {
        try { if (((42.ToString("X")).Length) != ((42.ToString("X")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    public static void Subject_10()
    {
        try { if (((Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)) ? 1 : 0) != ((Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    public static void Subject_11()
    {
        try { if (((Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)) ? 1 : 0) != ((Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Enum::BoxToString:System.String()
    public static void Subject_12()
    {
        try { if (((DayOfWeek.Monday.BoxToString()).Length) != ((DayOfWeek.Monday.BoxToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}