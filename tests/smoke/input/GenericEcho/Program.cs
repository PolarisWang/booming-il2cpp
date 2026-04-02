internal sealed class EchoBox<T>
{
    public EchoBox(T value)
    {
        Value = value;
    }

    public T Value { get; }
}

internal static class GenericEcho
{
    public static T Echo<T>(T value)
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
        EchoBox<string> textBox = new(GenericEcho.Echo("roadmap0"));
        EchoBox<int> numberBox = new(GenericEcho.Echo(42));
        (string Left, string Right) duplicated = GenericEcho.Duplicate(textBox.Value);

        Console.WriteLine(textBox.Value);
        Console.WriteLine(numberBox.Value);
        Console.WriteLine($"{duplicated.Left}:{duplicated.Right}");
        return 0;
    }
}
