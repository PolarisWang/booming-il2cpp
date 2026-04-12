using System;
using System.Diagnostics;

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
    public static int Main(string[] args)
    {
        int iterations = args.Length > 0 && int.TryParse(args[0], out var n) ? n : 1000;

        Shape[] shapes = { new Circle(3.0), new Rectangle(4.0, 5.0), new Circle(1.5), new Rectangle(2.0, 3.0) };
        var sw = Stopwatch.StartNew();
        double sum = 0;
        for (int i = 0; i < iterations * 100; i++)
            sum += shapes[i % shapes.Length].Area();
        sw.Stop();

        double opsPerSec = (iterations * 100.0) / (sw.Elapsed.TotalMilliseconds / 1000.0);
        Console.WriteLine($"{{\"elapsedMilliseconds\":{sw.Elapsed.TotalMilliseconds:F3},\"iterations\":{iterations},\"opsPerSecond\":{opsPerSec:F0},\"checksum\":{(long)(sum % 10000)}}}");
        return 0;
    }
}
