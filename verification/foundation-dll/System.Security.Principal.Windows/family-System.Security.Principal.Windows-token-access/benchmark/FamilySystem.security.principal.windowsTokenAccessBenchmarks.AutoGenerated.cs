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

// Auto-generated benchmark skeletons for FamilySystem.security.principal.windowsTokenAccessBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.security.principal.windowsTokenAccessBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.SafeAccessTokenHandle: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::SafeAccessTokenHandle::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_SafeAccessTokenHandle_Void_System_Ob() { _ = SafeAccessTokenHandle.SafeAccessTokenHandle:(42); }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Property_System_Void() { SafeAccessTokenHandle.Property; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_Method1_System_Void_P0() { SafeAccessTokenHandle.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_Method2_System_Void_P0_P1() { SafeAccessTokenHandle.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_Validate_System_Void_P0() { SafeAccessTokenHandle.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.TokenAccessLevels: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::TokenAccessLevels::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_TokenAccessLevels_Void_System_Object() { _ = TokenAccessLevels.TokenAccessLevels:(42); }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Property_System_Void() { TokenAccessLevels.Property; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_Method1_System_Void_P0() { TokenAccessLevels.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_Method2_System_Void_P0_P1() { TokenAccessLevels.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_Validate_System_Void_P0() { TokenAccessLevels.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Value_System_Void() { SafeAccessTokenHandle.Value; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_Value_System_Void_P0() { SafeAccessTokenHandle.Value = null!; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Count_System_Void() { SafeAccessTokenHandle.Count; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_Count_System_Void_P0() { SafeAccessTokenHandle.Count = null!; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_IsEmpty_System_Void() { SafeAccessTokenHandle.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_IsEmpty_System_Void_P0() { SafeAccessTokenHandle.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_get_Key_System_Void() { SafeAccessTokenHandle.Key; }
    // Purpose: Benchmark native-runtime performance of SafeAccessTokenHandle.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/Microsoft.Win32.SafeHandles.SafeAccessTokenHandle::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_Microsoft_Win32_SafeHandles_SafeAccessTokenHandle_set_Key_System_Void_P0() { SafeAccessTokenHandle.Key = null!; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Value_System_Void() { TokenAccessLevels.Value; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_Value_System_Void_P0() { TokenAccessLevels.Value = null!; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Count_System_Void() { TokenAccessLevels.Count; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_Count_System_Void_P0() { TokenAccessLevels.Count = null!; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_IsEmpty_System_Void() { TokenAccessLevels.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_IsEmpty_System_Void_P0() { TokenAccessLevels.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_get_Key_System_Void() { TokenAccessLevels.Key; }
    // Purpose: Benchmark native-runtime performance of TokenAccessLevels.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.TokenAccessLevels::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_TokenAccessLevels_set_Key_System_Void_P0() { TokenAccessLevels.Key = null!; }}
