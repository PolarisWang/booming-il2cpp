using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal class ArrayCovarianceBaseBox
{
    public int Value;
}

internal sealed class ArrayCovarianceDerivedBox : ArrayCovarianceBaseBox
{
}

internal static class ArrayCovarianceProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "array-covariance-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.ArrayCovariance,
        Priority = 5)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        object boxedArray = new ArrayCovarianceDerivedBox[1];
        var matched = boxedArray as ArrayCovarianceBaseBox[];
        var casted = (ArrayCovarianceBaseBox[])boxedArray;
        var objectArray = (object[])boxedArray;
        casted[0] = new ArrayCovarianceDerivedBox { Value = 42 };
        Assert.Equal(1, matched is null ? 0 : 1);
        Assert.Equal(1, objectArray is null ? 0 : 1);
        Assert.Equal(42, ((ArrayCovarianceDerivedBox)matched![0]).Value);
        Assert.Equal(42, ((ArrayCovarianceDerivedBox)casted[0]).Value);
        Assert.Equal(42, ((ArrayCovarianceDerivedBox)objectArray[0]).Value);
        return ChaosAssertState.Complete();
    }
}
