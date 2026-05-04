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

// Auto-generated skeleton for Guid Random HashCode (family/System.Private.CoreLib/guid/random/hashcode).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class GuidRandomHashcode
{
    // Guid..ctor
    [MethodSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Guid_ctor_System_Void_System_String()
    {
    // TODO: needs-manual — .ctor with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    public static void Benchmark_System_Private_CoreLib_System_Guid_ctor_System_Void_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Guid_ctor_System_Void_System_String() { }

    // Guid..ctor
    [MethodSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte()
    {
    // TODO: needs-manual — .ctor with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    public static void Benchmark_System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte() { }

    // Guid.NewGuid
    [MethodSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Guid_NewGuid_System_Guid()
    {
    var result = Guid.NewGuid();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Guid_NewGuid_System_Guid() { _ = Guid.NewGuid(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Guid_NewGuid_System_Guid() { _ = Guid.NewGuid(); }

    // Guid.Parse
    [MethodSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    public static void Benchmark_System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String() { }

    // Guid.TryParse
    [MethodSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid()
    {
    // TODO: needs-manual — TryParse with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    public static void Benchmark_System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid() { }

    // Guid.ToString
    [MethodSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Guid_ToString_System_String()
    {
    var result = Guid.NewGuid().ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Guid_ToString_System_String() { _ = Guid.NewGuid().ToString(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Guid_ToString_System_String() { _ = Guid.NewGuid().ToString(); }

    // Random..ctor
    [MethodSubjectId("System.Private.CoreLib/System.Random::.ctor:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Random_ctor_System_Void()
    {
    new Random();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Random::.ctor:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Random_ctor_System_Void() { new Random(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::.ctor:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Random_ctor_System_Void() { new Random(); }

    // Random.Next
    [MethodSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Random_Next_System_Int32()
    {
    var result = new Random().Next();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Random_Next_System_Int32() { _ = new Random().Next(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Random_Next_System_Int32() { _ = new Random().Next(); }

    // Random.Next
    [MethodSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32()
    {
    var result = new Random().Next(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32() { _ = new Random().Next(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32() { _ = new Random().Next(42); }

    // Random.NextDouble
    [MethodSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Random_NextDouble_System_Double()
    {
    var result = new Random().NextDouble();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Random_NextDouble_System_Double() { _ = new Random().NextDouble(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Random_NextDouble_System_Double() { _ = new Random().NextDouble(); }

    // Random.NextBytes
    [MethodSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte()
    {
    new Random().NextBytes(new byte[] { 1, 2, 3 });
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte() { new Random().NextBytes(new byte[] { 1, 2, 3 }); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte() { new Random().NextBytes(new byte[] { 1, 2, 3 }); }

    // HashCode.Add
    [MethodSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object()
    {
    default(HashCode).Add(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object() { default(HashCode).Add(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object() { default(HashCode).Add(42); }

    // HashCode.ToHashCode
    [MethodSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32()
    {
    var result = default(HashCode).ToHashCode();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32() { _ = default(HashCode).ToHashCode(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32() { _ = default(HashCode).ToHashCode(); }

    // HashCode.Combine
    [MethodSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32()
    {
    var result = HashCode.Combine(42, 42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32() { _ = HashCode.Combine(42, 42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32() { _ = HashCode.Combine(42, 42); }
}
