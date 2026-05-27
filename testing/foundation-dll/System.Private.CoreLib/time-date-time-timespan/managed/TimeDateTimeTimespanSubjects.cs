// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/time/date-time-timespan
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
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
        try { var _ = ((DateTime.Parse("2024-01-01")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)
    public static void Subject_3()
    {
        try { var _ = ((DateTime.TryParse("2024-01-01", out _)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.DateTime::ToString:System.String()
    public static void Subject_4()
    {
        try { var _ = ((DateTime.UtcNow.ToString()).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)
    public static void Subject_5()
    {
        try { var _ = ((DateTime.UtcNow.ToString("hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)
    public static void Subject_6()
    {
        try { var _ = ((DateTime.UtcNow.AddDays(42.0)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)
    public static void Subject_7()
    {
        try { var _ = ((DateTime.UtcNow.AddHours(42.0)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)
    public static void Subject_8()
    {
        try { var _ = ((DateTime.UtcNow.AddMinutes(42.0)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)
    public static void Subject_9()
    {
        try { var _ = DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)
    public static void Subject_10()
    {
        try { var _ = DateTime.DaysInMonth(2024, 2); }
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
        try { var _ = (long)((TimeSpan.FromDays(42.0)).Ticks); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)
    public static void Subject_13()
    {
        try { var _ = (long)((TimeSpan.FromHours(42.0)).Ticks); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)
    public static void Subject_14()
    {
        try { var _ = (long)((TimeSpan.FromMinutes(42.0)).Ticks); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)
    public static void Subject_15()
    {
        try { var _ = (long)((TimeSpan.Parse("1:00:00")).Ticks); }
        catch { _exitCode = 1; }
    }

}