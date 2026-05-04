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

// Auto-generated skeleton for Type Runtime Handles (family/System.Private.CoreLib/type/runtime-handles).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class TypeRuntimeHandles
{
    // Type.GetType
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_GetType_System_Type_System_String()
    {
    // TODO: needs-manual — GetType with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetType_System_Type_System_String() { }

    // Type.GetType
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean()
    {
    // TODO: needs-manual — GetType with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean() { }

    // Type.GetType
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean()
    {
    // TODO: needs-manual — GetType with 3 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean() { }

    // Type.GetTypeFromHandle
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle()
    {
    _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle() { _ = Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); }

    // Type.get_IsValueType
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean()
    {
    var result = typeof(byte).IsValueType;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean() { _ = typeof(byte).IsValueType; }

    // Type.get_IsEnum
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean()
    {
    var result = typeof(byte).IsEnum;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean() { _ = typeof(byte).IsEnum; }

    // Type.get_IsArray
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean()
    {
    var result = typeof(byte).IsArray;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean() { _ = typeof(byte).IsArray; }

    // Type.get_TypeHandle
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle()
    {
    _ = typeof(byte).TypeHandle;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle() { _ = typeof(byte).TypeHandle; }

    // Type.get_FullName
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_FullName_System_String()
    {
    var result = typeof(byte).FullName;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_FullName_System_String() { _ = typeof(byte).FullName; }

    // Type.get_Name
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_Name_System_String()
    {
    var result = typeof(byte).Name;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_Name_System_String() { _ = typeof(byte).Name; }

    // Type.get_BaseType
    [MethodSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_get_BaseType_System_Type()
    {
    var result = typeof(byte).BaseType;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_get_BaseType_System_Type() { _ = typeof(byte).BaseType; }

    // Type.GetMethods
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo()
    {
    _ = typeof(byte).GetMethods();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo() { _ = typeof(byte).GetMethods(); }

    // Type.GetFields
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo()
    {
    _ = typeof(byte).GetFields();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo() { _ = typeof(byte).GetFields(); }
}
