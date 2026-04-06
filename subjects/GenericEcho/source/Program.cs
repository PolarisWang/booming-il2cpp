using System;

namespace GenericEcho;

internal sealed class EchoBox<T>
{
    public EchoBox(T value)
    {
        Value = value;
    }

    public T Value { get; }
}

internal static class Echo
{
    public static T Return<T>(T value)
    {
        return value;
    }

    public static (T Left, T Right) Duplicate<T>(T value)
    {
        return (value, value);
    }
}

internal static class Program
{
    private static int Main()
    {
        var textBox = new EchoBox<string>(Echo.Return("roadmap0"));
        var numberBox = new EchoBox<int>(Echo.Return(42));
        var duplicated = Echo.Duplicate(textBox.Value);

        Console.WriteLine(textBox.Value);
        Console.WriteLine(numberBox.Value);
        Console.WriteLine($"{duplicated.Left}:{duplicated.Right}");
        return 0;
    }
}
