using System;

namespace BenchDispatch;

internal abstract class Shape
{
    public abstract double Area();
}

internal sealed class Circle : Shape
{
    private readonly double _r;
    public Circle(double r) => _r = r;
    public override double Area() => Math.PI * _r * _r;
}

internal sealed class Rectangle : Shape
{
    private readonly double _w, _h;
    public Rectangle(double w, double h) { _w = w; _h = h; }
    public override double Area() => _w * _h;
}

internal static class Program
{
    private const int IterationCount = 1000;
    private static long s_lastChecksum;

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static long RunWorkload()
    {
        Shape[] shapes = { new Circle(3.0), new Rectangle(4.0, 5.0), new Circle(1.5), new Rectangle(2.0, 3.0) };
        double sum = 0;
        for (int i = 0; i < IterationCount * 100; i++)
        {
            sum += shapes[i % shapes.Length].Area();
        }

        return (long)(sum % 10000);
    }
}
