// Auto-generated SDK benchmark skeleton
// Do not modify — re-generate via foundation-dll pipeline

using Chaos.TestFramework;

namespace GenericSupplementSubjectsBenchmarks;

[CapabilityTest("family/System.Private.CoreLib/generic/supplement")]
public class GenericSupplementSubjectsBenchmarks
{
    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestSupplementalMetadataRoundtrip:System.Int32()")]
    public void Subject_0()
    {
        GenericSupplementSubjects.Subject_0();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupBySubjectId:System.Int32()")]
    public void Subject_1()
    {
        GenericSupplementSubjects.Subject_1();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestTypeLookupByToken:System.Int32()")]
    public void Subject_2()
    {
        GenericSupplementSubjects.Subject_2();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("GenericSupplementNativeEntry/GenericSupplementNativeEntry::TestGenericInstantiation:System.Int32()")]
    public void Subject_3()
    {
        GenericSupplementSubjects.Subject_3();
    }

}