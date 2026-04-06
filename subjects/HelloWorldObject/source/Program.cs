namespace HelloWorldObject;

internal sealed class Greeter
{
    private readonly string _name;

    public Greeter(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return $"Hello, {_name}!";
    }
}

internal static class Program
{
    private static int Main(string[] args)
    {
        var greeter = new Greeter("World");
        Console.WriteLine(greeter.BuildMessage());
        return 0;
    }
}
