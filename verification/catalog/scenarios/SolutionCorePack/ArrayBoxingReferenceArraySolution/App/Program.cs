using System;

namespace ArrayBoxingReferenceArray.App;

internal static class BoxingSink
{
    public static void Consume(object value)
    {
        _ = value;
    }
}

internal sealed class ArrayBanner
{
    private readonly string _name;

    public ArrayBanner(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return string.Concat(
            string.Concat("Array boxing native proof: ", _name),
            ".");
    }
}

internal static class ArrayBoxingProofEntry
{
    public static int Run()
    {
        BoxingSink.Consume(42);
        var banners = new ArrayBanner[1];
        banners[0] = new ArrayBanner("boxed int 42");
        Console.WriteLine(banners[0].BuildMessage());
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return ArrayBoxingProofEntry.Run();
    }
}
