using System;

namespace InterfaceDispatchMessage.App;

internal interface IMessageBanner
{
    string Render();
}

internal sealed class InterfaceBanner : IMessageBanner
{
    private readonly string _value;

    public InterfaceBanner(string value)
    {
        _value = value;
    }

    public string Render()
    {
        return string.Concat(
            string.Concat("interface-dispatch-message:", _value),
            "|System.Runtime");
    }
}

internal static class InterfaceDispatchProofEntry
{
    public static int Run()
    {
        IMessageBanner banner = new InterfaceBanner("System.Private.CoreLib");
        Console.WriteLine(banner.Render());
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return InterfaceDispatchProofEntry.Run();
    }
}
