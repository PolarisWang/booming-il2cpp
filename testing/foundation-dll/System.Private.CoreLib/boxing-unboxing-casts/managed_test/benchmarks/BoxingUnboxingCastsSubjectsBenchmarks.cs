// Auto-generated SDK benchmark skeleton
// Do not modify — re-generate via foundation-dll pipeline

using Chaos.TestFramework;

namespace BoxingUnboxingCastsSubjectsBenchmarks;

[CapabilityTest("family/System.Private.CoreLib/boxing/unboxing-casts")]
public class BoxingUnboxingCastsSubjectsBenchmarks
{
    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    public void Subject_0()
    {
        BoxingUnboxingCastsSubjects.Subject_0();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    public void Subject_1()
    {
        BoxingUnboxingCastsSubjects.Subject_1();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    public void Subject_2()
    {
        BoxingUnboxingCastsSubjects.Subject_2();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    public void Subject_3()
    {
        BoxingUnboxingCastsSubjects.Subject_3();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    public void Subject_4()
    {
        BoxingUnboxingCastsSubjects.Subject_4();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    public void Subject_5()
    {
        BoxingUnboxingCastsSubjects.Subject_5();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    public void Subject_6()
    {
        BoxingUnboxingCastsSubjects.Subject_6();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    public void Subject_7()
    {
        BoxingUnboxingCastsSubjects.Subject_7();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    public void Subject_8()
    {
        BoxingUnboxingCastsSubjects.Subject_8();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    public void Subject_9()
    {
        BoxingUnboxingCastsSubjects.Subject_9();
    }

    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
        WarmupCount = 1, IterationCount = 100, InvocationCount = 1)]
    [BenchmarkSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    public void Subject_10()
    {
        BoxingUnboxingCastsSubjects.Subject_10();
    }

}