// Auto-generated SDK benchmark skeleton
// Do not modify — re-generate via foundation-dll pipeline

using Chaos.TestFramework;

namespace DispatchBasicSubjectsBenchmarks;

[CapabilityTest("family/System.Private.CoreLib/dispatch/basic")]
public class DispatchBasicSubjectsBenchmarks
{
    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)")]
    public void Subject_0()
    {
        DispatchBasicSubjects.Subject_0();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()")]
    public void Subject_1()
    {
        DispatchBasicSubjects.Subject_1();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()")]
    public void Subject_2()
    {
        DispatchBasicSubjects.Subject_2();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()")]
    public void Subject_3()
    {
        DispatchBasicSubjects.Subject_3();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()")]
    public void Subject_4()
    {
        DispatchBasicSubjects.Subject_4();
    }

}