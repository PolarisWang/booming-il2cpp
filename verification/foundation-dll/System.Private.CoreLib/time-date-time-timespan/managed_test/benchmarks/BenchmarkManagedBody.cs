// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/time/date-time-timespan
using System;

namespace Chaos.Benchmark.time_date_time_timespan
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(new DateTime(2024, 1, 1));
        }

        // System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(new DateTime(2024, 1, 1, 0, 0, 0));
        }

        // System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)
        public static void System_Private_CoreLib_System_DateTime_Parse_System_DateTime_System_String()
        {
            BenchmarkChecksum += ((DateTime.Parse("hello")).GetHashCode());
        }

        // System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)
        public static void System_Private_CoreLib_System_DateTime_TryParse_System_Boolean_System_String_System_DateTime_Ref()
        {
            BenchmarkChecksum += ((DateTime.TryParse("hello", DateTime.UtcNow)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.DateTime::ToString:System.String()
        public static void System_Private_CoreLib_System_DateTime_ToString_System_String()
        {
            BenchmarkChecksum += ((DateTime.UtcNow.ToString()).Length);
        }

        // System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)
        public static void System_Private_CoreLib_System_DateTime_ToString_System_String_System_String()
        {
            BenchmarkChecksum += ((DateTime.UtcNow.ToString("hello")).Length);
        }

        // System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)
        public static void System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double()
        {
            BenchmarkChecksum += ((DateTime.UtcNow.AddDays(42.0)).GetHashCode());
        }

        // System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)
        public static void System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double()
        {
            BenchmarkChecksum += ((DateTime.UtcNow.AddHours(42.0)).GetHashCode());
        }

        // System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)
        public static void System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double()
        {
            BenchmarkChecksum += ((DateTime.UtcNow.AddMinutes(42.0)).GetHashCode());
        }

        // System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)
        public static void System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime()
        {
            BenchmarkChecksum += DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow);
        }

        // System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_DateTime_DaysInMonth_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += DateTime.DaysInMonth(2024, 2);
        }

        // System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_TimeSpan_ctor_System_Void_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(new TimeSpan(42, 42, 42));
        }

        // System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)
        public static void System_Private_CoreLib_System_TimeSpan_FromDays_System_TimeSpan_System_Double()
        {
            BenchmarkChecksum += ((TimeSpan.FromDays(42.0)).GetHashCode());
        }

        // System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)
        public static void System_Private_CoreLib_System_TimeSpan_FromHours_System_TimeSpan_System_Double()
        {
            BenchmarkChecksum += ((TimeSpan.FromHours(42.0)).GetHashCode());
        }

        // System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)
        public static void System_Private_CoreLib_System_TimeSpan_FromMinutes_System_TimeSpan_System_Double()
        {
            BenchmarkChecksum += ((TimeSpan.FromMinutes(42.0)).GetHashCode());
        }

        // System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)
        public static void System_Private_CoreLib_System_TimeSpan_Parse_System_TimeSpan_System_String()
        {
            BenchmarkChecksum += ((TimeSpan.Parse("hello")).GetHashCode());
        }

    }
}
