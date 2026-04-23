using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeBenchmarks;

internal sealed class MemberMetadataLookupBenchmarkTarget
{
    public int Count;

    public MemberMetadataLookupBenchmarkTarget(int count)
    {
        Count = count;
    }

    public string Format(string prefix)
    {
        return prefix + ":" + Count;
    }
}

internal static class MemberMetadataLookupBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "member-metadata-lookup-bench",
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MemberMetadataLookup,
        Requires = ChaosRuntimeFeature.Reflection,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        Type type = typeof(MemberMetadataLookupBenchmarkTarget);
        int checksum = 0;

        for (int i = 0; i < 64; i++)
        {
            FieldInfo field = type.GetField(nameof(MemberMetadataLookupBenchmarkTarget.Count))!;
            MethodInfo method = type.GetMethod(nameof(MemberMetadataLookupBenchmarkTarget.Format), BindingFlags.Instance | BindingFlags.Public)!;
            checksum += field.Name.Length + method.Name.Length;
        }

        return checksum;
    }
}
