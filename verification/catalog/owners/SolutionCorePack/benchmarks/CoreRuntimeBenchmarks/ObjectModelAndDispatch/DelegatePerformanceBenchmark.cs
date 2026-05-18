using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

// ── Delegate types for C1: arity 0-4 ────────────────────────────────
internal delegate int  D0();
internal delegate int  D1(int a);
internal delegate int  D2(int a, int b);
internal delegate int  D3(int a, int b, int c);
internal delegate int  D4(int a, int b, int c, int d);

// ── Delegate types for C3: arity 5-8 (method_invoke fallback) ──────
internal delegate int  D5(int a, int b, int c, int d, int e);
internal delegate int  D6(int a, int b, int c, int d, int e, int f);
internal delegate int  D7(int a, int b, int c, int d, int e, int f, int g);
internal delegate int  D8(int a, int b, int c, int d, int e, int f, int g, int h);

internal static class DelegatePerformanceBenchmarkEntry
{
    private const int IterationCount = 100_000;

    // ── Static target helpers ───────────────────────────────────────
    private static int Static0() => 1;
    private static int Static1(int a) => a;
    private static int Static2(int a, int b) => a + b;
    private static int Static3(int a, int b, int c) => a + b + c;
    private static int Static4(int a, int b, int c, int d) => a + b + c + d;
    private static int Static5(int a, int b, int c, int d, int e) => a + b + c + d + e;
    private static int Static6(int a, int b, int c, int d, int e, int f) => a + b + c + d + e + f;
    private static int Static7(int a, int b, int c, int d, int e, int f, int g) => a + b + c + d + e + f + g;
    private static int Static8(int a, int b, int c, int d, int e, int f, int g, int h) => a + b + c + d + e + f + g + h;

    // ── Instance target ─────────────────────────────────────────────
    private sealed class Accumulator
    {
        private readonly int _offset;
        public Accumulator(int offset) { _offset = offset; }
        public int Add0() => _offset;
        public int Add1(int a) => _offset + a;
        public int Add2(int a, int b) => _offset + a + b;
        public int Add3(int a, int b, int c) => _offset + a + b + c;
        public int Add4(int a, int b, int c, int d) => _offset + a + b + c + d;
    }

    private static readonly D0 s_d0_static = Static0;
    private static readonly D1 s_d1_static = Static1;
    private static readonly D2 s_d2_static = Static2;
    private static readonly D3 s_d3_static = Static3;
    private static readonly D4 s_d4_static = Static4;

    private static readonly Accumulator s_acc = new(100);
    private static readonly D0 s_d0_instance = s_acc.Add0;
    private static readonly D1 s_d1_instance = s_acc.Add1;
    private static readonly D2 s_d2_instance = s_acc.Add2;
    private static readonly D3 s_d3_instance = s_acc.Add3;
    private static readonly D4 s_d4_instance = s_acc.Add4;

    private static readonly D5 s_d5_static = Static5;
    private static readonly D6 s_d6_static = Static6;
    private static readonly D7 s_d7_static = Static7;
    private static readonly D8 s_d8_static = Static8;

    // ═════════════════════════════════════════════════════════════════
    //  C1: Single-entry invocation — arity 0-4, static target
    // ═════════════════════════════════════════════════════════════════

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-static-arity0",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity0_Static()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d0_static();
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-static-arity1",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity1_Static()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d1_static(i);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-static-arity2",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity2_Static()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d2_static(i, i + 1);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-static-arity3",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity3_Static()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d3_static(i, i + 1, i + 2);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-static-arity4",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity4_Static()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d4_static(i, i + 1, i + 2, i + 3);
        return checksum;
    }

    // ═════════════════════════════════════════════════════════════════
    //  C1: Single-entry invocation — arity 0-4, instance target
    // ═════════════════════════════════════════════════════════════════

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-instance-arity0",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity0_Instance()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d0_instance();
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-instance-arity1",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity1_Instance()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d1_instance(i);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-instance-arity2",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity2_Instance()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d2_instance(i, i + 1);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-instance-arity3",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity3_Instance()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d3_instance(i, i + 1, i + 2);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c1-instance-arity4",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C1_Arity4_Instance()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d4_instance(i, i + 1, i + 2, i + 3);
        return checksum;
    }

    // ═════════════════════════════════════════════════════════════════
    //  C3: High-arity invocation — arity 5-8 (method_invoke fallback)
    // ═════════════════════════════════════════════════════════════════

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c3-arity5",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C3_Arity5()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d5_static(i, i + 1, i + 2, i + 3, i + 4);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c3-arity6",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C3_Arity6()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d6_static(i, i + 1, i + 2, i + 3, i + 4, i + 5);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c3-arity7",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C3_Arity7()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d7_static(i, i + 1, i + 2, i + 3, i + 4, i + 5, i + 6);
        return checksum;
    }

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-c3-arity8",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateInvocation,
        Requires = ChaosRuntimeFeature.Delegate,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int C3_Arity8()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
            checksum += s_d8_static(i, i + 1, i + 2, i + 3, i + 4, i + 5, i + 6, i + 7);
        return checksum;
    }
}
