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

public static partial class RuntimeCompilerServicesPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — RuntimeHelpers
    // ============================================================

    /// <summary>
    /// Patch 侧验证 RuntimeHelpers.GetHashCode 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RuntimeHelpers_GetHashCode()
    {
        _ = RuntimeHelpers.GetHashCode(new object());
    }

    /// <summary>
    /// Patch 侧验证 RuntimeHelpers.GetObjectValue 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RuntimeHelpers_GetObjectValue()
    {
        _ = RuntimeHelpers.GetObjectValue(42);
    }

    /// <summary>
    /// Patch 侧验证 RuntimeWrappedException..ctor 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RuntimeWrappedException_Ctor()
    {
        _ = new RuntimeWrappedException(42);
    }
}
