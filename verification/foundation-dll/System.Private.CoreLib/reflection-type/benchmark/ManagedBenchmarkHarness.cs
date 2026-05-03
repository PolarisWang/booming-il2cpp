// Auto-generated managed benchmark harness
// Family: family/System.Private.CoreLib/reflection/type
using System;
using System.Diagnostics;
using Chaos.Benchmark.reflection_type;

const int kWarmupIterations = 100;
const int kMeasureIterations = 10000;

var methodSubjects = new (string SubjectId, Action Body)[]
{
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String),
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean),
    ("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean),
    ("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle),
    ("System.Private.CoreLib/System.Type::get_Name:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_Name_System_String),
    ("System.Private.CoreLib/System.Type::get_FullName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_FullName_System_String),
    ("System.Private.CoreLib/System.Type::get_Namespace:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_Namespace_System_String),
    ("System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_AssemblyQualifiedName_System_String),
    ("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle),
    ("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsAbstract_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsSealed_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsInterface_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsArray_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsByRef_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsPointer_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsNested_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsPublic_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsNotPublic_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsNestedPublic_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_IsNestedPrivate_System_Boolean),
    ("System.Private.CoreLib/System.Type::get_BaseType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_BaseType_System_Type),
    ("System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_DeclaringType_System_Type),
    ("System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_ReflectedType_System_Type),
    ("System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_UnderlyingSystemType_System_Type),
    ("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo),
    ("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo),
    ("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo),
    ("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetEvents_System_Reflection_EventInfo),
    ("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo),
    ("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo_System_Reflection_BindingFlags),
    ("System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetMembers_System_Reflection_MemberInfo),
    ("System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetInterfaces_System_Type),
    ("System.Private.CoreLib/System.Type::GetElementType:System.Type()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetElementType_System_Type),
    ("System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetNestedTypes_System_Type),
    ("System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)", BenchmarkManagedBody.System_Private_CoreLib_System_Type_GetNestedType_System_Type_System_String),
    ("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()", BenchmarkManagedBody.System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly)
};

Console.WriteLine("{");
Console.WriteLine("  \"schemaVersion\": 1,");
Console.WriteLine("  \"assemblyName\": \"System.Private.CoreLib\",");
Console.WriteLine("  \"familyId\": \"family/System.Private.CoreLib/reflection/type\",");
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