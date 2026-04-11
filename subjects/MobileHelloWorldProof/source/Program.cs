using System;

namespace MobileHelloWorldProof;

internal sealed class MobileBanner
{
    private readonly string _name;

    public MobileBanner(string name)
    {
        _name = name;
    }

    public string BuildMessage()
    {
        return "Mobile native proof: " + _name + ".";
    }
}

internal static class Program
{
    private static int Main(string[] args)
    {
        var banner = new MobileBanner("hello world");
        Console.WriteLine(banner.BuildMessage());
        return 0;
    }
}
