using Chaos.TestFramework;

namespace PerformanceFeaturePack;

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
        WarmupCount = 1,
        IterationCount = 6,
        InvocationCount = 1)]
    public static long RunWorkload()
    {
        DispatchShape[] shapes =
        {
            new DispatchCircle(3.0),
            new DispatchRectangle(4.0, 5.0),
            new DispatchCircle(1.5),
            new DispatchRectangle(2.0, 3.0),
        };

        double sum = 0;
        for (int i = 0; i < IterationCount * 100; i++)
        {
            sum += shapes[i % shapes.Length].Area();
        }

        return (long)(sum % 10000);
    }
}
