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

public static partial class MathNumericsPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — Math 函数
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Math.Abs 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Math_Abs()
    {
        _ = Math.Abs(42);
    }

    /// <summary>
    /// Patch 侧验证 Math.Sqrt 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Math_Sqrt()
    {
        _ = Math.Sqrt(42.0);
    }

    /// <summary>
    /// Patch 侧验证 Math.Round 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Math::Round:System.Double(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_MathNumerics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Math_Round()
    {
        _ = Math.Round(3.5);
    }
}
