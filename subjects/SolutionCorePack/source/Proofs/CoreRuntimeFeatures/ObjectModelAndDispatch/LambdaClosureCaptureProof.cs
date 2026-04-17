using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class LambdaClosureCaptureBox
{
    private readonly int _seed;

    public LambdaClosureCaptureBox(int seed)
    {
        _seed = seed;
    }

    public Func<int, int> CreateAdder()
    {
        return value => value + _seed;
    }
}

internal static class LambdaClosureCaptureProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "lambda-closure-capture-proof",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.LambdaClosureCapture,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 4)]
    public static int Run()
    {
        var box = new LambdaClosureCaptureBox(21);
        Func<int, int> addSeed = box.CreateAdder();
        var result = addSeed(21);

        Assert.Equal(42, result);
        return 0;
    }
}
