using Chaos.IL2CPP.EngineBinding;
using Chaos.IL2CPP.EngineBinding.Smoke;

var generated = EngineBindingStubEmitter.GenerateForInterface(typeof(SmokeHostExports));

if (args.Length == 2 && string.Equals(args[0], "--check-baseline", StringComparison.Ordinal))
{
    var baselinePath = Path.GetFullPath(args[1]);
    var baselineSource = File.ReadAllText(baselinePath);

    if (!string.Equals(Normalize(generated), Normalize(baselineSource), StringComparison.Ordinal))
    {
        Console.Error.WriteLine($"Engine binding baseline mismatch: {baselinePath}");
        return 1;
    }

    Console.WriteLine($"Engine binding baseline matches: {baselinePath}");
    return 0;
}

Console.Write(generated);
return 0;

static string Normalize(string value)
{
    return value.Replace("\r\n", "\n", StringComparison.Ordinal)
        .Replace("\r", "\n", StringComparison.Ordinal)
        .Trim();
}
