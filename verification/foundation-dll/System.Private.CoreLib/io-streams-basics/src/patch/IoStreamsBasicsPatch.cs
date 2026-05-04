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

public static partial class IoStreamsBasicsPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — Stream 操作
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Stream.Flush 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::Flush:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Stream_Flush()
    {
        using var ms = new MemoryStream();
        ms.Flush();
    }

    /// <summary>
    /// Patch 侧验证 Stream.get_Length 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Stream_GetLength()
    {
        using var ms = new MemoryStream();
        _ = ms.Length;
    }

    /// <summary>
    /// Patch 侧验证 TextReader.ReadToEnd 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_IOStreamsBasics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_TextReader_ReadToEnd()
    {
        using var reader = new StringReader("hello");
        _ = reader.ReadToEnd();
    }
}
