namespace GenericEchoClosedMinimal;

internal sealed class EchoBox<T>
{
    private readonly T _value;

    public EchoBox(T value)
    {
        _value = value;
    }

    public T Value => _value;
}

internal static class GenericEcho
{
    public static T Echo<T>(T value)
    {
        return value;
    }
}

internal static class Program
{
    private static int Main()
    {
        Console.WriteLine(new EchoBox<string>(GenericEcho.Echo("generic-ok")).Value);
        return 0;
    }
}
