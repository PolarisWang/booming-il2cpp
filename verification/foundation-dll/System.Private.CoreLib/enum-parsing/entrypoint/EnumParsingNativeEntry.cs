// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/enum/parsing
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class EnumParsingNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
    static int Method0()
    {
        return (Enum.Format(typeof(DayOfWeek), DayOfWeek.Monday, "G")?.GetHashCode() ?? 0);
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    static int Method1()
    {
        return (Enum.GetName(typeof(DayOfWeek), DayOfWeek.Monday)?.GetHashCode() ?? 0);
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    static int Method2()
    {
        return (Enum.GetName(typeof(DayOfWeek), DayOfWeek.Monday)?.GetHashCode() ?? 0);
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    static int Method3()
    {
        return Enum.GetNames(typeof(DayOfWeek)).GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    static int Method4()
    {
        return Enum.GetValues(typeof(DayOfWeek)).GetHashCode();
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    static int Method5()
    {
        return (Enum.IsDefined(typeof(DayOfWeek), "Monday") ? 1 : 0);
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    static int Method6()
    {
        return (Enum.Parse(typeof(DayOfWeek), "Monday")?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    static int Method7()
    {
        return (Enum.Parse(typeof(DayOfWeek), "Monday", true)?.GetHashCode() ?? 0);
    }

    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    static int Method8()
    {
        return (DayOfWeek.Monday.ToString()?.GetHashCode() ?? 0);
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    static int Method9()
    {
        // TODO: System.Private.CoreLib/System.Enum::ToString:System.String(System.String) could not be auto-generated
        return 0;
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    static int Method10()
    {
        return (Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object __ep0) ? 1 : 0);
    }

    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    static int Method11()
    {
        return (Enum.TryParse(typeof(DayOfWeek), "Monday", out object __ep0) ? 1 : 0);
    }

}