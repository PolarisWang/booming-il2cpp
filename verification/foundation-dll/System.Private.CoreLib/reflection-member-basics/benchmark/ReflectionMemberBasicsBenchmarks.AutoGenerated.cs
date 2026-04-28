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

// Auto-generated benchmark skeletons for ReflectionMemberBasicsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionMemberBasicsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { }
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_MemberType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { }
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_DeclaringType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { }
    // Purpose: Benchmark native-runtime performance of MethodBase.Invoke with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_Invoke_System_Object_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of MethodInfo.GetParameters with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodInfo_GetParameters_System_Reflection_ParameterInfo() { }
    // Purpose: Benchmark native-runtime performance of MethodInfo.get_ReturnType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { }
    // Purpose: Benchmark native-runtime performance of ConstructorInfo.Invoke with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of FieldInfo.GetValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_GetValue_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of FieldInfo.SetValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of FieldInfo.get_FieldType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.GetValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.GetValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.SetValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object() { }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.get_PropertyType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberBasics)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { }}
