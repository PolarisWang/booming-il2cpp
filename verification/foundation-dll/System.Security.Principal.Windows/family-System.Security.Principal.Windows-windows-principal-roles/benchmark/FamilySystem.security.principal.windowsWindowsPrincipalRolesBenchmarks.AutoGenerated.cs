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

// Auto-generated benchmark skeletons for FamilySystem.security.principal.windowsWindowsPrincipalRolesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.security.principal.windowsWindowsPrincipalRolesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.WindowsAccountType: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::WindowsAccountType::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_WindowsAccountType_Void_System_Object() { _ = WindowsAccountType.WindowsAccountType:(42); }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Property_System_Void() { WindowsAccountType.Property; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_Method1_System_Void_P0() { WindowsAccountType.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_Method2_System_Void_P0_P1() { WindowsAccountType.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_Validate_System_Void_P0() { WindowsAccountType.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.WindowsBuiltInRole: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::WindowsBuiltInRole::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_WindowsBuiltInRole_Void_System_Object() { _ = WindowsBuiltInRole.WindowsBuiltInRole:(42); }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Property_System_Void() { WindowsBuiltInRole.Property; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_Method1_System_Void_P0() { WindowsBuiltInRole.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_Method2_System_Void_P0_P1() { WindowsBuiltInRole.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_Validate_System_Void_P0() { WindowsBuiltInRole.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.WindowsPrincipal: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::WindowsPrincipal::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_WindowsPrincipal_Void_System_Object() { _ = WindowsPrincipal.WindowsPrincipal:(42); }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Property_System_Void() { WindowsPrincipal.Property; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_Method1_System_Void_P0() { WindowsPrincipal.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_Method2_System_Void_P0_P1() { WindowsPrincipal.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_Validate_System_Void_P0() { WindowsPrincipal.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Value_System_Void() { WindowsAccountType.Value; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_Value_System_Void_P0() { WindowsAccountType.Value = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Count_System_Void() { WindowsAccountType.Count; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_Count_System_Void_P0() { WindowsAccountType.Count = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_IsEmpty_System_Void() { WindowsAccountType.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_IsEmpty_System_Void_P0() { WindowsAccountType.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_get_Key_System_Void() { WindowsAccountType.Key; }
    // Purpose: Benchmark native-runtime performance of WindowsAccountType.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsAccountType::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsAccountType_set_Key_System_Void_P0() { WindowsAccountType.Key = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Value_System_Void() { WindowsBuiltInRole.Value; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_Value_System_Void_P0() { WindowsBuiltInRole.Value = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Count_System_Void() { WindowsBuiltInRole.Count; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_Count_System_Void_P0() { WindowsBuiltInRole.Count = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_IsEmpty_System_Void() { WindowsBuiltInRole.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_IsEmpty_System_Void_P0() { WindowsBuiltInRole.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_get_Key_System_Void() { WindowsBuiltInRole.Key; }
    // Purpose: Benchmark native-runtime performance of WindowsBuiltInRole.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsBuiltInRole::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsBuiltInRole_set_Key_System_Void_P0() { WindowsBuiltInRole.Key = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Value_System_Void() { WindowsPrincipal.Value; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_Value_System_Void_P0() { WindowsPrincipal.Value = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Count_System_Void() { WindowsPrincipal.Count; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_Count_System_Void_P0() { WindowsPrincipal.Count = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_IsEmpty_System_Void() { WindowsPrincipal.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_IsEmpty_System_Void_P0() { WindowsPrincipal.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_get_Key_System_Void() { WindowsPrincipal.Key; }
    // Purpose: Benchmark native-runtime performance of WindowsPrincipal.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsPrincipal_set_Key_System_Void_P0() { WindowsPrincipal.Key = null!; }}
