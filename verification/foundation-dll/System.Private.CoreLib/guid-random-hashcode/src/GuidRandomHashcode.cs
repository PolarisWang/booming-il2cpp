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

public partial class GuidRandomHashcode
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试方法
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GuidNewGuid_Handwrite()
    {
        _ = Guid.NewGuid();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GuidToString_Handwrite()
    {
        _ = Guid.NewGuid().ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RandomNextBytes_Handwrite()
    {
        var bytes = new byte[256];
        new Random().NextBytes(bytes);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_HashCodeCombine_Handwrite()
    {
        _ = HashCode.Combine(42, 99);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新宿主侧方法 (HostToPatch)
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_GuidParse_HostToPatch()
    {
        _ = Guid.Parse("A1B2C3D4-E5F6-7890-ABCD-EF1234567890");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_RandomNext_HostToPatch()
    {
        _ = new Random().Next();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_RandomNextBounded_HostToPatch()
    {
        _ = new Random().Next(1000);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_HashCodeToHashCode_HostToPatch()
    {
        HashCode hc = default;
        hc.Add("host-to-patch");
        _ = hc.ToHashCode();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_GuidTryParse_HostToPatch()
    {
        _ = Guid.TryParse("A1B2C3D4-E5F6-7890-ABCD-EF1234567890", out _);
    }
}
