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

// Auto-generated skeleton for Object Equality And Identity (family/System.Private.CoreLib/object/equality-identity).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ObjectEqualityIdentity
{
    // Object.Equals
    [MethodSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object()
    {
    var result = new object().Equals(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object() { _ = new object().Equals(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object() { _ = new object().Equals(42); }

    // Object.Equals
    [MethodSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object()
    {
    var result = Object.Equals(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object() { _ = Object.Equals(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object() { _ = Object.Equals(42, 42); }

    // Object.ReferenceEquals
    [MethodSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object()
    {
    var result = Object.ReferenceEquals(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object() { _ = Object.ReferenceEquals(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object() { _ = Object.ReferenceEquals(42, 42); }

    // Object.GetHashCode
    [MethodSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_GetHashCode_System_Int32()
    {
    var result = new object().GetHashCode();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_GetHashCode_System_Int32() { _ = new object().GetHashCode(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_GetHashCode_System_Int32() { _ = new object().GetHashCode(); }

    // Object.ToString
    [MethodSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_ToString_System_String()
    {
    var result = new object().ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_ToString_System_String() { _ = new object().ToString(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_ToString_System_String() { _ = new object().ToString(); }

    // Object.GetType
    [MethodSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_GetType_System_Type()
    {
    var result = new object().GetType();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Object_GetType_System_Type() { _ = new object().GetType(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_GetType_System_Type() { _ = new object().GetType(); }

    // Object.MemberwiseClone
    [MethodSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object()
    {
    // TODO: needs-manual — MemberwiseClone requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    public static void Benchmark_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object() { }
}
