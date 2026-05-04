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

// Auto-generated skeleton for Boxing Unboxing Casts (family/System.Private.CoreLib/boxing/unboxing-casts).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class BoxingUnboxingCasts
{
    // Convert.ChangeType
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type()
    {
    var result = Convert.ChangeType(42, typeof(byte));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type() { _ = Convert.ChangeType(42, typeof(byte)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type() { _ = Convert.ChangeType(42, typeof(byte)); }

    // Convert.ChangeType
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider()
    {
    _ = Convert.ChangeType(42, typeof(byte), null!);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider() { _ = Convert.ChangeType(42, typeof(byte), null!); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider() { _ = Convert.ChangeType(42, typeof(byte), null!); }

    // Type.IsAssignableFrom
    [MethodSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type()
    {
    var result = typeof(byte).IsAssignableFrom(typeof(byte));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableFrom(typeof(byte)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableFrom(typeof(byte)); }

    // Type.IsInstanceOfType
    [MethodSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object()
    {
    var result = typeof(byte).IsInstanceOfType(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object() { _ = typeof(byte).IsInstanceOfType(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object() { _ = typeof(byte).IsInstanceOfType(42); }

    // Type.IsSubclassOf
    [MethodSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type()
    {
    var result = typeof(byte).IsSubclassOf(typeof(byte));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type() { _ = typeof(byte).IsSubclassOf(typeof(byte)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type() { _ = typeof(byte).IsSubclassOf(typeof(byte)); }

    // Type.IsAssignableTo
    [MethodSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type()
    {
    var result = typeof(byte).IsAssignableTo(typeof(byte));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableTo(typeof(byte)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type() { _ = typeof(byte).IsAssignableTo(typeof(byte)); }

    // Object.MemberwiseClone
    [MethodSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object()
    {
    // TODO: needs-manual — MemberwiseClone requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    public static void Benchmark_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }

    // RuntimeHelpers.GetObjectValue
    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object()
    {
    var result = RuntimeHelpers.GetObjectValue(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object() { _ = RuntimeHelpers.GetObjectValue(42); }

    // Array.CreateInstance
    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42); }

    // Array.CreateInstance
    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), 42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32() { _ = Array.CreateInstance(typeof(byte), 42, 42); }

    // ValueType.Equals
    [MethodSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object()
    {
    var result = ((ValueType)42).Equals(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object() { _ = ((ValueType)42).Equals(42); }
}
