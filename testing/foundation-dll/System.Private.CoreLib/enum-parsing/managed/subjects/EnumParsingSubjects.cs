// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/enum/parsing
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
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
        try { var _ = ((Enum.Format(typeof(byte), null!, "hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    public static void Subject_1()
    {
        try { var _ = ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    public static void Subject_2()
    {
        try { var _ = ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    public static void Subject_3()
    {
        try { var _ = ((Enum.GetNames(typeof(byte))).Length); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    public static void Subject_4()
    {
        try { var _ = (int)(((Array)(Enum.GetValues(typeof(byte)))).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    public static void Subject_5()
    {
        try { var _ = ((Enum.IsDefined(typeof(byte), null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    public static void Subject_8()
    {
        try { var _ = ((Enum.Format(typeof(byte), (byte)42, "G")).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    public static void Subject_9()
    {
        try { var _ = ((Enum.Format(typeof(byte), (byte)42, "X")).Length); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    // [12] System.Private.CoreLib/System.Enum::BoxToString:System.String()
}