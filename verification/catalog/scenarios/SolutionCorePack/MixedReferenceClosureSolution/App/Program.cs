using System.Collections.Immutable;
using GoldenMixedReference.Core;
using ReferenceGreeter;

namespace GoldenMixedReference.App;

internal static class Program
{
    private static int Main()
    {
        var immutable = ImmutableArray.Create("mixed", "closure");
        var projectMessage = MessageComposer.Compose(immutable[0], immutable[1]);
        var binaryMessage = Greeter.ComposeSuffix("bundle", "mix");
        var combined = $"{projectMessage}|{binaryMessage}";
        Console.WriteLine(combined);
        return string.Equals(combined, "mixed-closure|bundle-mix", StringComparison.Ordinal) ? 0 : 1;
    }
}
