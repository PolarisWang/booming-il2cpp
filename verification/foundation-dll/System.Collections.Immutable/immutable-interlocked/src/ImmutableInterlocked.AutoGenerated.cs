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

// Auto-generated skeleton for Immutable Interlocked (family/System.Collections.Immutable/immutable/interlocked).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ImmutableInterlocked
{
    // ImmutableInterlocked.Update<string>
    [MethodSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableInterlocked)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Update_string_System_Boolean_System_Strin()
    {
    var refLocal_0 = "hello";
    var result = ImmutableInterlocked.Update<string>(ref refLocal_0, null!, null!);
    }

    [BenchmarkSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableInterlocked)]
    public static void Benchmark_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Update_string_System_Boolean_System_Strin() { }

    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableInterlocked::Update<string>:System.Boolean(System.String&,System.Func{System.String,System.String})")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemCollectionsImmutable_ImmutableInterlocked)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableInterlocked_Update_string_System_Boolean_System_Strin() { }
}
