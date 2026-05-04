using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Auto-generated skeleton for Date Time TimeSpan (family/System.Private.CoreLib/time/date-time-timespan).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class TimeDateTimeTimespan
{
    // DateTime..ctor
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32()
    {
    new DateTime(2024, 1, 1);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32() { new DateTime(2024, 1, 1); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32() { new DateTime(2024, 1, 1); }

    // DateTime..ctor
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32()
    {
    new DateTime(2024, 1, 1, 0, 0, 0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32() { new DateTime(2024, 1, 1, 0, 0, 0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32() { new DateTime(2024, 1, 1, 0, 0, 0); }

    // DateTime.Parse
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_Parse_System_DateTime_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_Parse_System_DateTime_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_Parse_System_DateTime_System_String() { }

    // DateTime.TryParse
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_TryParse_System_Boolean_System_String_System_DateTime()
    {
    // TODO: needs-manual — TryParse with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_TryParse_System_Boolean_System_String_System_DateTime() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_TryParse_System_Boolean_System_String_System_DateTime() { }

    // DateTime.ToString
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_ToString_System_String()
    {
    var result = DateTime.UtcNow.ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_ToString_System_String() { _ = DateTime.UtcNow.ToString(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_ToString_System_String() { _ = DateTime.UtcNow.ToString(); }

    // DateTime.ToString
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_ToString_System_String_System_String()
    {
    var result = DateTime.UtcNow.ToString("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_ToString_System_String_System_String() { _ = DateTime.UtcNow.ToString("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_ToString_System_String_System_String() { _ = DateTime.UtcNow.ToString("hello"); }

    // DateTime.AddDays
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double()
    {
    var result = DateTime.UtcNow.AddDays(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddDays(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddDays(42.0); }

    // DateTime.AddHours
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double()
    {
    var result = DateTime.UtcNow.AddHours(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddHours(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddHours(42.0); }

    // DateTime.AddMinutes
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double()
    {
    var result = DateTime.UtcNow.AddMinutes(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddMinutes(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddMinutes(42.0); }

    // DateTime.Compare
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime()
    {
    var result = DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime() { _ = DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime() { _ = DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow); }

    // DateTime.DaysInMonth
    [MethodSubjectId("System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_DateTime_DaysInMonth_System_Int32_System_Int32_System_Int32()
    {
    var result = DateTime.DaysInMonth(2024, 2);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_DateTime_DaysInMonth_System_Int32_System_Int32_System_Int32() { _ = DateTime.DaysInMonth(2024, 2); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_DateTime_DaysInMonth_System_Int32_System_Int32_System_Int32() { _ = DateTime.DaysInMonth(2024, 2); }

    // TimeSpan..ctor
    [MethodSubjectId("System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_TimeSpan_ctor_System_Void_System_Int32_System_Int32_System_Int32()
    {
    new TimeSpan(42, 42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_TimeSpan_ctor_System_Void_System_Int32_System_Int32_System_Int32() { new TimeSpan(42, 42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_TimeSpan_ctor_System_Void_System_Int32_System_Int32_System_Int32() { new TimeSpan(42, 42, 42); }

    // TimeSpan.FromDays
    [MethodSubjectId("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_TimeSpan_FromDays_System_TimeSpan_System_Double()
    {
    var result = TimeSpan.FromDays(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_TimeSpan_FromDays_System_TimeSpan_System_Double() { _ = TimeSpan.FromDays(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_TimeSpan_FromDays_System_TimeSpan_System_Double() { _ = TimeSpan.FromDays(42.0); }

    // TimeSpan.FromHours
    [MethodSubjectId("System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_TimeSpan_FromHours_System_TimeSpan_System_Double()
    {
    var result = TimeSpan.FromHours(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_TimeSpan_FromHours_System_TimeSpan_System_Double() { _ = TimeSpan.FromHours(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_TimeSpan_FromHours_System_TimeSpan_System_Double() { _ = TimeSpan.FromHours(42.0); }

    // TimeSpan.FromMinutes
    [MethodSubjectId("System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_TimeSpan_FromMinutes_System_TimeSpan_System_Double()
    {
    var result = TimeSpan.FromMinutes(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_TimeSpan_FromMinutes_System_TimeSpan_System_Double() { _ = TimeSpan.FromMinutes(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_TimeSpan_FromMinutes_System_TimeSpan_System_Double() { _ = TimeSpan.FromMinutes(42.0); }

    // TimeSpan.Parse
    [MethodSubjectId("System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_TimeSpan_Parse_System_TimeSpan_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    public static void Benchmark_System_Private_CoreLib_System_TimeSpan_Parse_System_TimeSpan_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_TimeSpan_Parse_System_TimeSpan_System_String() { }
}
