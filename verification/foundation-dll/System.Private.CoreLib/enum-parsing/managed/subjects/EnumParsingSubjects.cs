// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/enum/parsing
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class EnumParsingSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
    public static void Subject_0()
    {
        try { Enum.Format(typeof(byte), 42, "hello"); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    public static void Subject_1()
    {
        if (((Enum.GetName(typeof(DayOfWeek), (object)1)).Length) != 6) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    public static void Subject_2()
    {
        if (((Enum.GetName(typeof(DayOfWeek), (object)1)).Length) != 6) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    public static void Subject_3()
    {
        try { Enum.GetNames(typeof(byte)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    public static void Subject_4()
    {
        try { Enum.GetValues(typeof(byte)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    public static void Subject_5()
    {
        try { Enum.IsDefined(typeof(byte), 42); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    public static void Subject_6()
    {
        try { Enum.Parse(typeof(byte), "hello"); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    public static void Subject_7()
    {
        try { Enum.Parse(typeof(byte), "hello", true); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    public static void Subject_8()
    {
        if (((DayOfWeek.Monday.ToString()).Length) != 6) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    public static void Subject_9()
    {
        if (((42.ToString("X")).Length) != 2) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    public static void Subject_10()
    {
        if (((Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    public static void Subject_11()
    {
        if (((Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)) ? 1 : 0) != 1) _exitCode = 1;
    }

}