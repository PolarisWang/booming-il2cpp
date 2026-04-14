namespace GoldenMixedReference.Core;

public static class MessageComposer
{
    public static string Compose(string left, string right)
    {
        return $"{left}-{right}";
    }
}
