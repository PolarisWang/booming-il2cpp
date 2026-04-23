using GoldenHotUpdate.SharedContracts;

namespace GoldenHotUpdate.PatchModule;

public sealed class PatchFeature : IPatchFeature
{
    public int GetPatchedValue()
    {
        return 42;
    }

    public string ComposeMessage(IHostMath hostMath, string name)
    {
        var token = hostMath.Add(name.Length, 2);
        return hostMath.Decorate($"{name}-{token}");
    }

    public T Echo<T>(T value)
    {
        return value;
    }

    public int Apply(Func<int, int> callback, int value)
    {
        return callback(value);
    }

    public void ThrowPatchFault()
    {
        throw new InvalidOperationException("patch-fault");
    }
}
