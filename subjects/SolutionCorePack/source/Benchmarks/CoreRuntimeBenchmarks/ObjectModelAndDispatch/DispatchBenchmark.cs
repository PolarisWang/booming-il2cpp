using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal abstract class DispatchShape
{
    public abstract double Area();
}

internal sealed class DispatchCircle : DispatchShape
{
    private readonly double _r;

    public DispatchCircle(double r)
    {
        _r = r;
    }

    public override double Area()
    {
        return Math.PI * _r * _r;
    }
}

internal sealed class DispatchRectangle : DispatchShape
{
    private readonly double _w;
    private readonly double _h;

    public DispatchRectangle(double w, double h)
    {
        _w = w;
        _h = h;
    }

    public override double Area()
    {
        return _w * _h;
    }
}

internal static class DispatchBenchmarkEntry
{
    private const int IterationCount = 1000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "dispatch-bench",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericVirtualDispatch,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        DispatchShape first = new DispatchCircle(3.0);
        DispatchShape second = new DispatchRectangle(4.0, 5.0);
        DispatchShape third = new DispatchCircle(1.5);
        DispatchShape fourth = new DispatchRectangle(2.0, 3.0);

        double sum = 0;
        for (int i = 0; i < IterationCount * 100; i++)
        {
            DispatchShape shape = (i & 3) switch
            {
                0 => first,
                1 => second,
                2 => third,
                _ => fourth,
            };
            sum += shape.Area();
        }

        return (int)(sum % 10000);
    }
}
