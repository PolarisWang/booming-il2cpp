// Dispatch subjects — virtual methods and interface dispatch
using System;

namespace VirtualDispatch;

// ── Interface hierarchy ──
public interface IShape
{
    string GetName();
    double Area();
}

public interface IColoredShape : IShape
{
    string GetColor();
}

// ── Base class with virtual ──
public class Shape
{
    public virtual string GetName() => "Shape";
}

// ── Derived override ──
public class Circle : Shape, IColoredShape
{
    private double _radius;
    public Circle(double radius) { _radius = radius; }

    public override string GetName() => "Circle";

    public double Area() => 3.14159 * _radius * _radius;

    public string GetColor() => "Red";
}

// ── Another derived class ──
public class Rectangle : Shape, IShape
{
    private double _w, _h;
    public Rectangle(double w, double h) { _w = w; _h = h; }

    public override string GetName() => "Rectangle";

    public double Area() => _w * _h;
}

public static class DispatchSubjects
{
    // Call virtual method through base reference
    public static string TestVirtualCall(Shape s) => s.GetName();

    // Call interface method through interface reference
    public static double TestInterfaceCall(IShape s) => s.Area();

    // Call interface method from derived interface
    public static string TestDerivedInterfaceCall(IColoredShape s) => s.GetColor();
}
