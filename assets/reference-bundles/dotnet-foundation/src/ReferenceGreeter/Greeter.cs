namespace ReferenceGreeter;

public static class Greeter
{
    public static string Compose(string value)
    {
        return $"hello-{value}";
    }

    public static string ComposeSuffix(string value, string suffix)
    {
        return $"{value}-{suffix}";
    }
}
