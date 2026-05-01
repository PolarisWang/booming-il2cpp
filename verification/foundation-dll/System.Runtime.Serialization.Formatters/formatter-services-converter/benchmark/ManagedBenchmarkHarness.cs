// Auto-generated managed benchmark harness
// Family: family/System.Runtime.Serialization.Formatters/formatter-services-converter
using System;
using System.Diagnostics;
using Chaos.Benchmark.formatter_services_converter;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::FormatterConverter::Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_FormatterConverter_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_Convert_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToBoolean:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToBoolean_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToByte:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToByte_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToChar:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToChar_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDateTime:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDateTime_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDecimal:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDecimal_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDouble:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDouble_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt16:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt16_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt32:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt32_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt64:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt64_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSByte:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSByte_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSingle:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSingle_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToString:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToString_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt16:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt16_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt32:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt32_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt64:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt64_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::CheckTypeSecurity:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_CheckTypeSecurity_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetObjectData:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetObjectData_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSafeUninitializedObject:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSafeUninitializedObject_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSurrogateForCyclicalReference:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSurrogateForCyclicalReference_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetTypeFromAssembly:System.Void(P0, P1)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetTypeFromAssembly_System_Void_P0_P1),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetUninitializedObject:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetUninitializedObject_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::PopulateObjectMembers:System.Void(P0, P1, P2)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_PopulateObjectMembers_System_Void_P0_P1_P2),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldNames_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldTypes_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldNames_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldTypes_System_Void_P0),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::SerializationObjectManager::Void(System.Object)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_SerializationObjectManager_Void_System_Object),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::RaiseOnSerializedEvent:System.Void()", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_RaiseOnSerializedEvent_System_Void),
    ("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::RegisterObject:System.Void(P0)", BenchmarkManagedBody.System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_RegisterObject_System_Void_P0)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Runtime.Serialization.Formatters\",");
Console.WriteLine("  \"familyId\": \"family/System.Runtime.Serialization.Formatters/formatter-services-converter\",");
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