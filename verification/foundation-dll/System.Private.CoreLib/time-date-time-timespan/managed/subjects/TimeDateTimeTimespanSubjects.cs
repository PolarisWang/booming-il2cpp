// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/time/date/time/timespan
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class TimeDateTimeTimespanSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    public static void Subject_0()
    {
        new DateTime(2024, 1, 1);
    }

    // [1] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)
    public static void Subject_1()
    {
        new DateTime(2024, 1, 1, 0, 0, 0);
    }

    // [2] System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)
    public static void Subject_2()
    {
        if (((DateTime.Parse("2024-01-01")).GetHashCode()) != -1847801252) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.DateTime::ToString:System.String()
    public static void Subject_4()
    {
        if (((DateTime.UtcNow.ToString()).Length) != 20) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)
    public static void Subject_5()
    {
        if (((DateTime.UtcNow.ToString("hello")).Length) != 5) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)
    public static void Subject_6()
    {
        if (((DateTime.UtcNow.AddDays(42.0)).GetHashCode()) != -1237043284) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)
    public static void Subject_7()
    {
        if (((DateTime.UtcNow.AddHours(42.0)).GetHashCode()) != -609958670) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)
    public static void Subject_8()
    {
        if (((DateTime.UtcNow.AddMinutes(42.0)).GetHashCode()) != -1349227924) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)
    public static void Subject_9()
    {
        if (DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow) != 0) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)
    public static void Subject_10()
    {
        if (DateTime.DaysInMonth(2024, 2) != 29) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    public static void Subject_11()
    {
        new TimeSpan(42, 42, 42);
    }

    // [12] System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)
    public static void Subject_12()
    {
        if (((TimeSpan.FromDays(42.0)).GetHashCode()) != -178675456) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)
    public static void Subject_13()
    {
        if (((TimeSpan.FromHours(42.0)).GetHashCode()) != 171512160) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)
    public static void Subject_14()
    {
        if (((TimeSpan.FromMinutes(42.0)).GetHashCode()) != -569803771) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)
    public static void Subject_15()
    {
        if (((TimeSpan.Parse("1:00:00")).GetHashCode()) != 1640261640) _exitCode = 1;
    }

}