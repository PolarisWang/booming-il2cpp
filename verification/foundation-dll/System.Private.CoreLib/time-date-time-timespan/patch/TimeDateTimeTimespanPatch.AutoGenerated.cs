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

// Auto-generated patch-side skeletons for TimeDateTimeTimespanPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class TimeDateTimeTimespanPatch
{
    // Purpose: Verify DateTime..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32() { new DateTime(2024, 1, 1); }
    // Purpose: Verify DateTime..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_ctor_System_Void_System_Int32_System_Int32_System_Int32_System_Int32_System_Int32() { new DateTime(2024, 1, 1, 0, 0, 0); }
    // Purpose: Verify DateTime.Parse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_Parse_System_DateTime_System_String() { }
    // Purpose: Verify DateTime.TryParse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_TryParse_System_Boolean_System_String_System_DateTime() { }
    // Purpose: Verify DateTime.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_ToString_System_String() { _ = DateTime.UtcNow.ToString(); }
    // Purpose: Verify DateTime.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_ToString_System_String_System_String() { _ = DateTime.UtcNow.ToString("hello"); }
    // Purpose: Verify DateTime.AddDays executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_AddDays_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddDays(42.0); }
    // Purpose: Verify DateTime.AddHours executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_AddHours_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddHours(42.0); }
    // Purpose: Verify DateTime.AddMinutes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_AddMinutes_System_DateTime_System_Double() { _ = DateTime.UtcNow.AddMinutes(42.0); }
    // Purpose: Verify DateTime.Compare executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_Compare_System_Int32_System_DateTime_System_DateTime() { _ = DateTime.Compare(DateTime.UtcNow, DateTime.UtcNow); }
    // Purpose: Verify DateTime.DaysInMonth executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_DateTime_DaysInMonth_System_Int32_System_Int32_System_Int32() { _ = DateTime.DaysInMonth(2024, 2); }
    // Purpose: Verify TimeSpan..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_TimeSpan_ctor_System_Void_System_Int32_System_Int32_System_Int32() { new TimeSpan(42, 42, 42); }
    // Purpose: Verify TimeSpan.FromDays executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_TimeSpan_FromDays_System_TimeSpan_System_Double() { _ = TimeSpan.FromDays(42.0); }
    // Purpose: Verify TimeSpan.FromHours executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_TimeSpan_FromHours_System_TimeSpan_System_Double() { _ = TimeSpan.FromHours(42.0); }
    // Purpose: Verify TimeSpan.FromMinutes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_TimeSpan_FromMinutes_System_TimeSpan_System_Double() { _ = TimeSpan.FromMinutes(42.0); }
    // Purpose: Verify TimeSpan.Parse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TimeDateTimeTimespan)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_TimeSpan_Parse_System_TimeSpan_System_String() { }}
