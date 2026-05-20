using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/reflection/member-complete
public static class ReflectionMemberCompleteBenchmarks
{
    // Benchmark MemberInfo.get_Name native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String() { _ = default(MemberInfo)!.Name; }
    // Benchmark MemberInfo.get_MemberType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes() { _ = typeof(byte).MemberType; }
    // Benchmark MemberInfo.get_DeclaringType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type() { _ = default(MemberInfo)!.DeclaringType; }
    // Benchmark MemberInfo.get_ReflectedType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_ReflectedType_System_Type() { _ = default(MemberInfo)!.ReflectedType; }
    // Benchmark MemberInfo.get_MetadataToken native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MemberInfo_get_MetadataToken_System_Int32() { _ = default(MemberInfo)!.MetadataToken; }
    // Benchmark MethodInfo.get_ReturnType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type() { _ = typeof(byte).GetMethods()[0].ReturnType; }
    // Benchmark MethodInfo.GetBaseDefinition native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodInfo_GetBaseDefinition_System_Reflection_MethodInfo() { _ = default(MethodInfo)!.GetBaseDefinition(); }
    // Benchmark MethodBase.get_MethodHandle native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_MethodHandle_System_RuntimeMethodHandle() { _ = default(MethodInfo)!.MethodHandle; }
    // Benchmark MethodBase.get_CallingConvention native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_CallingConvention_System_Reflection_CallingConventions() { _ = default(MethodInfo)!.CallingConvention; }
    // Benchmark MethodBase.get_IsPublic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_IsPublic_System_Boolean() { _ = default(MethodInfo)!.IsPublic; }
    // Benchmark MethodBase.get_IsStatic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_IsStatic_System_Boolean() { _ = default(MethodInfo)!.IsStatic; }
    // Benchmark MethodBase.get_IsVirtual native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_get_IsVirtual_System_Boolean() { _ = default(MethodInfo)!.IsVirtual; }
    // Benchmark FieldInfo.get_FieldType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type() { _ = typeof(byte).GetFields()[0].FieldType; }
    // Benchmark FieldInfo.get_IsStatic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsStatic_System_Boolean() { _ = default(FieldInfo)!.IsStatic; }
    // Benchmark FieldInfo.get_IsInitOnly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsInitOnly_System_Boolean() { _ = default(FieldInfo)!.IsInitOnly; }
    // Benchmark FieldInfo.get_IsLiteral native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsLiteral_System_Boolean() { _ = default(FieldInfo)!.IsLiteral; }
    // Benchmark PropertyInfo.get_PropertyType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type() { _ = typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].PropertyType; }
    // Benchmark PropertyInfo.get_CanRead native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanRead_System_Boolean() { _ = default(PropertyInfo)!.CanRead; }
    // Benchmark PropertyInfo.get_CanWrite native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanWrite_System_Boolean() { _ = default(PropertyInfo)!.CanWrite; }
    // Benchmark PropertyInfo.GetIndexParameters native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetIndexParameters_System_Reflection_ParameterInfo() { _ = default(PropertyInfo)!.GetIndexParameters(); }
    // Benchmark FieldInfo.get_FieldHandle native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldHandle_System_RuntimeFieldHandle() { _ = default(FieldInfo)!.FieldHandle; }
    // Benchmark ConstructorInfo.get_CallingConvention native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ConstructorInfo_get_CallingConvention_System_Reflection_CallingConventions() { _ = default(ConstructorInfo)!.CallingConvention; }}
