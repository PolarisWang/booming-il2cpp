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

// Auto-generated benchmark skeletons for FamilySystem.security.principal.windowsWindowsIdentityBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.security.principal.windowsWindowsIdentityBenchmarks
{
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.WindowsIdentity: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::WindowsIdentity::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_WindowsIdentity_Void_System_Object() { _ = WindowsIdentity.WindowsIdentity:(42); }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Property_System_Void() { WindowsIdentity.Property; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_Method1_System_Void_P0() { WindowsIdentity.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_Method2_System_Void_P0_P1() { WindowsIdentity.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_Validate_System_Void_P0() { WindowsIdentity.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Value_System_Void() { WindowsIdentity.Value; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_Value_System_Void_P0() { WindowsIdentity.Value = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Count_System_Void() { WindowsIdentity.Count; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_Count_System_Void_P0() { WindowsIdentity.Count = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_IsEmpty_System_Void() { WindowsIdentity.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_IsEmpty_System_Void_P0() { WindowsIdentity.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_get_Key_System_Void() { WindowsIdentity.Key; }
    // Purpose: Benchmark native-runtime performance of WindowsIdentity.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_WindowsIdentity_set_Key_System_Void_P0() { WindowsIdentity.Key = null!; }}
