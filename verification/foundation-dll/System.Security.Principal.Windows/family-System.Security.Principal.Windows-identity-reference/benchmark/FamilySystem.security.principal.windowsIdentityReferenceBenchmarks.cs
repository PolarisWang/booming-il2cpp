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

// Auto-generated benchmark skeletons for FamilySystem.security.principal.windowsIdentityReferenceBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.security.principal.windowsIdentityReferenceBenchmarks
{
    // Purpose: Benchmark native-runtime performance of IdentityReference.IdentityReference: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IdentityReference::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_IdentityReference_Void_System_Object() { _ = IdentityReference.IdentityReference:(42); }
    // Purpose: Benchmark native-runtime performance of IdentityReference.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Property_System_Void() { IdentityReference.Property; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Method1_System_Void_P0() { IdentityReference.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of IdentityReference.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Method2_System_Void_P0_P1() { IdentityReference.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IdentityReference.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_Validate_System_Void_P0() { IdentityReference.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.IdentityReferenceCollection: with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::IdentityReferenceCollection::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_IdentityReferenceCollection_Void() { _ = IdentityReferenceCollection.IdentityReferenceCollection:(42); }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.get_Property with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Property_System_Void() { IdentityReferenceCollection.Property; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.Method1 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Method1_System_Void_P0() { IdentityReferenceCollection.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.Method2 with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Method2_System_Void_P0_P1() { IdentityReferenceCollection.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.Validate with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_Validate_System_Void_P0() { IdentityReferenceCollection.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of IdentityReference.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Value_System_Void() { IdentityReference.Value; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Value_System_Void_P0() { IdentityReference.Value = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Count_System_Void() { IdentityReference.Count; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Count_System_Void_P0() { IdentityReference.Count = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_IsEmpty_System_Void() { IdentityReference.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_IsEmpty_System_Void_P0() { IdentityReference.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_get_Key_System_Void() { IdentityReference.Key; }
    // Purpose: Benchmark native-runtime performance of IdentityReference.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReference::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReference_set_Key_System_Void_P0() { IdentityReference.Key = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.get_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Value_System_Void() { IdentityReferenceCollection.Value; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.set_Value with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Value_System_Void_P0() { IdentityReferenceCollection.Value = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.get_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Count_System_Void() { IdentityReferenceCollection.Count; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.set_Count with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Count_System_Void_P0() { IdentityReferenceCollection.Count = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_IsEmpty_System_Void() { IdentityReferenceCollection.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_IsEmpty_System_Void_P0() { IdentityReferenceCollection.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.get_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_get_Key_System_Void() { IdentityReferenceCollection.Key; }
    // Purpose: Benchmark native-runtime performance of IdentityReferenceCollection.set_Key with typical input
    [BenchmarkSubjectId("System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Security_Principal_Windows_System_Security_Principal_IdentityReferenceCollection_set_Key_System_Void_P0() { IdentityReferenceCollection.Key = null!; }}
