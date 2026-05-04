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

public partial class IoStreamsBasics
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — Stream 操作
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Stream_Flush()
    {
        using var ms = new MemoryStream();
        ms.Flush();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Stream_Seek()
    {
        using var ms = new MemoryStream();
        _ = ms.Seek(0, SeekOrigin.Begin);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_TextReader_ReadToEnd()
    {
        using var reader = new StringReader("hello");
        _ = reader.ReadToEnd();
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — Stream 操作
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Stream_SetLength()
    {
        using var ms = new MemoryStream();
        ms.SetLength(42L);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_TextWriter_Write()
    {
        using var sw = new StringWriter();
        sw.Write("hello");
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_TextReader_ReadLine()
    {
        using var reader = new StringReader("hello");
        _ = reader.ReadLine();
    }
}
