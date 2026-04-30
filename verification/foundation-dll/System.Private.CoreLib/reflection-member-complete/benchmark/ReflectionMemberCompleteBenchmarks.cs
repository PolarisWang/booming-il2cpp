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

// Auto-generated benchmark skeletons for ReflectionMemberCompleteBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionMemberCompleteBenchmarks
{
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { _ = typeof(string).GetMembers()[0].Name; }
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_MemberType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { _ = typeof(string).GetMembers()[0].MemberType; }
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_DeclaringType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { _ = typeof(string).GetMembers()[0].DeclaringType; }
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_ReflectedType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_ReflectedType_System_Type() { _ = typeof(string).GetMembers()[0].ReflectedType; }
    // Purpose: Benchmark native-runtime performance of MemberInfo.get_MetadataToken with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_MetadataToken_System_Int32() { _ = typeof(string).GetMembers()[0].MetadataToken; }
    // Purpose: Benchmark native-runtime performance of MethodInfo.get_ReturnType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.ReturnType; }
    // Purpose: Benchmark native-runtime performance of MethodInfo.GetBaseDefinition with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodInfo_GetBaseDefinition_System_Reflection_MethodInfo() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetBaseDefinition(); }
    // Purpose: Benchmark native-runtime performance of MethodBase.get_MethodHandle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_MethodHandle_System_RuntimeMethodHandle() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.MethodHandle; }
    // Purpose: Benchmark native-runtime performance of MethodBase.get_CallingConvention with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_CallingConvention_System_Reflection_CallingConventions() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.CallingConvention; }
    // Purpose: Benchmark native-runtime performance of MethodBase.get_IsPublic with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_IsPublic_System_Boolean() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsPublic; }
    // Purpose: Benchmark native-runtime performance of MethodBase.get_IsStatic with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_IsStatic_System_Boolean() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsStatic; }
    // Purpose: Benchmark native-runtime performance of MethodBase.get_IsVirtual with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_IsVirtual_System_Boolean() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsVirtual; }
    // Purpose: Benchmark native-runtime performance of FieldInfo.get_FieldType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { _ = typeof(string).GetField("Empty")!.FieldType; }
    // Purpose: Benchmark native-runtime performance of FieldInfo.get_IsStatic with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsStatic_System_Boolean() { _ = typeof(string).GetField("Empty")!.IsStatic; }
    // Purpose: Benchmark native-runtime performance of FieldInfo.get_IsInitOnly with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsInitOnly_System_Boolean() { _ = typeof(string).GetField("Empty")!.IsInitOnly; }
    // Purpose: Benchmark native-runtime performance of FieldInfo.get_IsLiteral with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsLiteral_System_Boolean() { _ = typeof(string).GetField("Empty")!.IsLiteral; }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.get_PropertyType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { _ = typeof(string).GetProperty("Length")!.PropertyType; }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.get_CanRead with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanRead_System_Boolean() { _ = typeof(string).GetProperty("Length")!.CanRead; }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.get_CanWrite with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanWrite_System_Boolean() { _ = typeof(string).GetProperty("Length")!.CanWrite; }
    // Purpose: Benchmark native-runtime performance of PropertyInfo.GetIndexParameters with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetIndexParameters_System_Reflection_ParameterInfo() { _ = typeof(string).GetProperty("Length")!.GetIndexParameters(); }
    // Purpose: Benchmark native-runtime performance of FieldInfo.get_FieldHandle with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldHandle_System_RuntimeFieldHandle() { _ = typeof(string).GetField("Empty")!.FieldHandle; }
    // Purpose: Benchmark native-runtime performance of ConstructorInfo.get_CallingConvention with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ConstructorInfo_get_CallingConvention_System_Reflection_CallingConventions() { _ = typeof(object).GetConstructor(System.Type.EmptyTypes)!.CallingConvention; }}
