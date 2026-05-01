// Auto-generated managed benchmark harness
// Family: family/System.Runtime.Serialization.Formatters/object-manager-id-generator
using System;
using System.Diagnostics;
using Chaos.Benchmark.object_manager_id_generator;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::ObjectIDGenerator::Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectIDGenerator_ObjectIDGenerator_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::GetId:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectIDGenerator_GetId_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::HasId:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectIDGenerator_HasId_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::ObjectManager::Void(System.Object, System.Object)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_ObjectManager_Void_System_Object_System_Object),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::DoFixups:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_DoFixups_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::GetObject:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_GetObject_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseDeserializationEvent:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RaiseDeserializationEvent_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseOnDeserializingEvent:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RaiseOnDeserializingEvent_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RecordArrayElementFixup_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordDelayedFixup:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RecordDelayedFixup_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordFixup:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RecordFixup_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2, P3, P4)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1_P2_P3_P4),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2, P3, P4, P5)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ObjectManager_RegisterObject_System_Void_P0_P1_P2_P3_P4_P5)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.Serialization.Formatters\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.Serialization.Formatters/object-manager-id-generator\",");
Console.WriteLine("  \"warmupIterations\": " + kWarmupIterations + ",");
Console.WriteLine("  \"measureIterations\": " + kMeasureIterations + ",");
Console.WriteLine("  \"results\": [");

for (int i = 0; i < methodSubjects.Length; i++)
{
    var (subjectId, body) = methodSubjects[i];

    // Warmup
    for (int w = 0; w < kWarmupIterations; w++)
    {
        body();
    }

    // Measurement
    var sw = Stopwatch.StartNew();
    for (int m = 0; m < kMeasureIterations; m++)
    {
        body();
    }
    sw.Stop();

    double elapsedMs = sw.Elapsed.TotalMilliseconds;
    double opsPerSecond = kMeasureIterations / (elapsedMs / 1000.0);

    string comma = (i < methodSubjects.Length - 1) ? "," : "";
    Console.WriteLine("    {");
    Console.WriteLine("      \"methodIndex\": " + i + ",");
    Console.WriteLine("      \"methodSubjectId\": \"" + subjectId.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\",");
    Console.WriteLine("      \"elapsedMilliseconds\": " + elapsedMs.ToString("F6") + ",");
    Console.WriteLine("      \"opsPerSecond\": " + opsPerSecond.ToString("F6") + ",");
    Console.WriteLine("      \"iterations\": " + kMeasureIterations);
    Console.WriteLine("    }" + comma);
}

Console.WriteLine("  ]");
Console.WriteLine("}");