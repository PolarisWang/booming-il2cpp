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

// Auto-generated patch-side skeletons for ImmutableInterlockedPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ImmutableInterlockedPatch
{
    // Purpose: Verify ImmutableInterlocked.Update<string> executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Update_string_System_Boolean_System_Strin() { }}
