// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/time/date-time-timespan
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class TimeDateTimeTimespanSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    public static void Subject_0()
    {
        try { new DateTime(2024, 1, 1); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)
    public static void Subject_1()
    {
        try { new DateTime(2024, 1, 1, 0, 0, 0); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)
    public static void Subject_2()
    {
        try { if (((DateTime.Parse("2024-01-01")).GetHashCode()) != ((DateTime.Parse("2024-01-01")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)
    public static void Subject_3()
    {
        try { if (((DateTime.TryParse("2024-01-01", out _)) ? 1 : 0) != ((DateTime.TryParse("2024-01-01", out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.DateTime::ToString:System.String()
    public static void Subject_4()
    {
        try { if (((DateTime.UtcNow.ToString()).Length) != ((DateTime.UtcNow.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)
    public static void Subject_5()
    {
        try { if (((DateTime.UtcNow.ToString("hello")).Length) != ((DateTime.UtcNow.ToString("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)
    public static void Subject_6()
    {
        try { if (((DateTime.UtcNow.AddDays(42.0)).GetHashCode()) != ((DateTime.UtcNow.AddDays(42.0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)
    public static void Subject_7()
    {
        try { if (((DateTime.UtcNow.AddHours(42.0)).GetHashCode()) != ((DateTime.UtcNow.AddHours(42.0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)
    public static void Subject_8()
    {
        try { if (((DateTime.UtcNow.AddMinutes(42.0)).GetHashCode()) != ((DateTime.UtcNow.AddMinutes(42.0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)
    public static void Subject_9()
    {
        try { if (DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow) != DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)
    public static void Subject_10()
    {
        try { if (DateTime.DaysInMonth(2024, 2) != DateTime.DaysInMonth(2024, 2)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    public static void Subject_11()
    {
        try { new TimeSpan(42, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)
    public static void Subject_12()
    {
        try { if (((TimeSpan.FromDays(42.0)).GetHashCode()) != ((TimeSpan.FromDays(42.0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)
    public static void Subject_13()
    {
        try { if (((TimeSpan.FromHours(42.0)).GetHashCode()) != ((TimeSpan.FromHours(42.0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)
    public static void Subject_14()
    {
        try { if (((TimeSpan.FromMinutes(42.0)).GetHashCode()) != ((TimeSpan.FromMinutes(42.0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)
    public static void Subject_15()
    {
        try { if (((TimeSpan.Parse("1:00:00")).GetHashCode()) != ((TimeSpan.Parse("1:00:00")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}