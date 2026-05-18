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
        if (((Enum.Format(typeof(byte), 42, "hello")).Length) != ((Enum.Format(typeof(byte), 42, "hello")).Length)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    public static void Subject_1()
    {
        if (((Enum.GetName(typeof(DayOfWeek), (object)1)).Length) != ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length)) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    public static void Subject_2()
    {
        if (((Enum.GetName(typeof(DayOfWeek), (object)1)).Length) != ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length)) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    public static void Subject_3()
    {
        if (((Enum.GetNames(typeof(byte))).Length) != ((Enum.GetNames(typeof(byte))).Length)) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    public static void Subject_4()
    {
        if (((Enum.GetValues(typeof(byte))).GetHashCode()) != ((Enum.GetValues(typeof(byte))).GetHashCode())) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    public static void Subject_5()
    {
        if (((Enum.IsDefined(typeof(byte), 42)) ? 1 : 0) != ((Enum.IsDefined(typeof(byte), 42)) ? 1 : 0)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    public static void Subject_6()
    {
        if (((Enum.Parse(typeof(byte), "hello")).GetHashCode()) != ((Enum.Parse(typeof(byte), "hello")).GetHashCode())) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    public static void Subject_7()
    {
        if (((Enum.Parse(typeof(byte), "hello", true)).GetHashCode()) != ((Enum.Parse(typeof(byte), "hello", true)).GetHashCode())) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    public static void Subject_8()
    {
        if (((DayOfWeek.Monday.ToString()).Length) != ((DayOfWeek.Monday.ToString()).Length)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    public static void Subject_9()
    {
        // TODO: System.Private.CoreLib/System.Enum::ToString:System.String(System.String) could not be auto-generated
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    public static void Subject_10()
    {
        // TODO: System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&) could not be auto-generated
    }

    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    public static void Subject_11()
    {
        // TODO: System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&) could not be auto-generated
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
        }
    }

}