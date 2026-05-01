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

// Auto-generated benchmark skeletons for FamilySystem.runtime.serialization.formattersObjectManagerIdGeneratorBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.serialization.formattersObjectManagerIdGeneratorBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ObjectIDGenerator.ObjectIDGenerator: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::ObjectIDGenerator::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectIDGenerator_ObjectIDGenerator_Void() { _ = ObjectIDGenerator.ObjectIDGenerator:(); }
    // Purpose: Benchmark native-runtime performance of ObjectIDGenerator.GetId with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::GetId:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectIDGenerator_GetId_System_Void_P0_P1() { ObjectIDGenerator.GetId(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectIDGenerator.HasId with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::HasId:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectIDGenerator_HasId_System_Void_P0_P1() { ObjectIDGenerator.HasId(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.ObjectManager: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::ObjectManager::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_ObjectManager_Void_System_Object_Syst() { _ = ObjectManager.ObjectManager:(42, 42); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.DoFixups with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::DoFixups:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_DoFixups_System_Void() { ObjectManager.DoFixups(); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.GetObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::GetObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_GetObject_System_Void_P0() { ObjectManager.GetObject(null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RaiseDeserializationEvent with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseDeserializationEvent:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RaiseDeserializationEvent_System_Void() { ObjectManager.RaiseDeserializationEvent(); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RaiseOnDeserializingEvent with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseOnDeserializingEvent:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RaiseOnDeserializingEvent_System_Void() { ObjectManager.RaiseOnDeserializingEvent(null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RecordArrayElementFixup with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RecordArrayElementFixup_System_Void_P() { ObjectManager.RecordArrayElementFixup(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RecordDelayedFixup with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordDelayedFixup:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RecordDelayedFixup_System_Void_P0_P1_() { ObjectManager.RecordDelayedFixup(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RecordFixup with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordFixup:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RecordFixup_System_Void_P0_P1_P2() { ObjectManager.RecordFixup(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RegisterObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1() { ObjectManager.RegisterObject(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RegisterObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1_P2() { ObjectManager.RegisterObject(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RegisterObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2, P3, P4)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1_P2_P() { ObjectManager.RegisterObject(null!, null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ObjectManager.RegisterObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2, P3, P4, P5)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1_P2_P() { ObjectManager.RegisterObject(null!, null!, null!, null!, null!, null!); }}
