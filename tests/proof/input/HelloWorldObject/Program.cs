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
    private static void Main(string[] args)
    {
        var greeter = new Greeter(args.Length > 0 ? args[0] : "object-world");
        Console.WriteLine(greeter.BuildMessage());
    }
}
