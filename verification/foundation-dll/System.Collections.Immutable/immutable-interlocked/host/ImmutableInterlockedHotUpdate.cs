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

// Hot-update stubs for family/System.Collections.Immutable/immutable/interlocked
public static class ImmutableInterlockedHotUpdate
{
    // Verify ImmutableInterlocked.Update<string> after hot-update (patch side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Update_string_System_Boolean_System_Strin() { }}
