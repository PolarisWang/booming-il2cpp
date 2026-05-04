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

public partial class StringCharTextCore
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试方法
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_StringCompare_Handwrite()
    {
        _ = string.Compare("The quick brown fox", "The quick brown fox");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_StringConcatThree_Handwrite()
    {
        _ = string.Concat("alpha", "beta", "gamma");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_StringFormat_Handwrite()
    {
        _ = string.Format("Int={0}, Str={1}, Double={2:F2}", 42, "hello", 3.14);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.String::Split:System.String[](System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_StringSplit_Handwrite()
    {
        _ = "a,b,c,d,e,f,g".Split(',');
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新宿主侧方法 (HostToPatch)
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_StringIndexOfChar_HostToPatch()
    {
        _ = "hello world".IndexOf('w');
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::ToUpper:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_StringToUpper_HostToPatch()
    {
        _ = "hello world".ToUpper();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.String::Trim:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_StringTrim_HostToPatch()
    {
        _ = "  padded text  ".Trim();
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_CharIsDigit_HostToPatch()
    {
        _ = char.IsDigit('7');
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_StringCharTextCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_CharIsLetter_HostToPatch()
    {
        _ = char.IsLetter('Z');
    }
}
