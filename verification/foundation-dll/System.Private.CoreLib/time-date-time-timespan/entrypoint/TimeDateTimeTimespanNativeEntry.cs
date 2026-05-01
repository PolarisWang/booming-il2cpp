// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/time/date/time/timespan
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class TimeDateTimeTimespanNativeEntry
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
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            case 15: return Method15();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    static int Method0()
    {
        new DateTime(2024, 1, 1);
        return (int)0;
    }

    // [1] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)
    static int Method1()
    {
        new DateTime(2024, 1, 1, 0, 0, 0);
        return (int)0;
    }

    // [2] System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)
    static int Method2()
    {
        // TODO: System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String) could not be auto-generated
        return 0;
    }

    // [3] System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)
    static int Method3()
    {
    var refLocal_1 = DateTime.UtcNow;
        return (int)(DateTime.TryParse("hello", ref refLocal_1));
    }

    // [4] System.Private.CoreLib/System.DateTime::ToString:System.String()
    static int Method4()
    {
        return (int)(DateTime.UtcNow.ToString());
    }

    // [5] System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)
    static int Method5()
    {
        return (int)(DateTime.UtcNow.ToString("hello"));
    }

    // [6] System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)
    static int Method6()
    {
        return (int)(DateTime.UtcNow.AddDays(42.0));
    }

    // [7] System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)
    static int Method7()
    {
        return (int)(DateTime.UtcNow.AddHours(42.0));
    }

    // [8] System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)
    static int Method8()
    {
        return (int)(DateTime.UtcNow.AddMinutes(42.0));
    }

    // [9] System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)
    static int Method9()
    {
        return DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow);
    }

    // [10] System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)
    static int Method10()
    {
        return DateTime.DaysInMonth(2024, 2);
    }

    // [11] System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    static int Method11()
    {
        new TimeSpan(42, 42, 42);
        return (int)0;
    }

    // [12] System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)
    static int Method12()
    {
        return (int)(TimeSpan.FromDays(42.0));
    }

    // [13] System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)
    static int Method13()
    {
        return (int)(TimeSpan.FromHours(42.0));
    }

    // [14] System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)
    static int Method14()
    {
        return (int)(TimeSpan.FromMinutes(42.0));
    }

    // [15] System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)
    static int Method15()
    {
        // TODO: System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String) could not be auto-generated
        return 0;
    }

}