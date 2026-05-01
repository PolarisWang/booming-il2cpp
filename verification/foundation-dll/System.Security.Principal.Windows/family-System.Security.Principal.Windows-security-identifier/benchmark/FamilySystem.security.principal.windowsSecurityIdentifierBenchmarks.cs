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

// Auto-generated benchmark skeletons for FamilySystem.security.principal.windowsSecurityIdentifierBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.security.principal.windowsSecurityIdentifierBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.SecurityIdentifier: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::SecurityIdentifier::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_SecurityIdentifier_Void_System_Object() { _ = SecurityIdentifier.SecurityIdentifier:(42); }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Property_System_Void() { SecurityIdentifier.Property; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Method1_System_Void_P0() { SecurityIdentifier.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Method2_System_Void_P0_P1() { SecurityIdentifier.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_Validate_System_Void_P0() { SecurityIdentifier.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Value_System_Void() { SecurityIdentifier.Value; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Value_System_Void_P0() { SecurityIdentifier.Value = null!; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Count_System_Void() { SecurityIdentifier.Count; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Count_System_Void_P0() { SecurityIdentifier.Count = null!; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_IsEmpty_System_Void() { SecurityIdentifier.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_IsEmpty_System_Void_P0() { SecurityIdentifier.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_get_Key_System_Void() { SecurityIdentifier.Key; }
    // Purpose: Benchmark native-runtime performance of SecurityIdentifier.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_SecurityIdentifier_set_Key_System_Void_P0() { SecurityIdentifier.Key = null!; }}
