using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal interface ICastTypeCheckView
{
}

internal class CastTypeCheckBaseBox
{
    public int BaseValue;
}

internal class CastTypeCheckMiddleBox : CastTypeCheckBaseBox
{
    public int MiddleValue;
}

internal sealed class CastTypeCheckDerivedBox : CastTypeCheckMiddleBox, ICastTypeCheckView
{
    public int DerivedValue;
}

internal static class CastTypeCheckProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "cast-type-check-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.ObjectCastAndTypeCheck,
        Priority = 3)]
    public static int Run()
    {
        ChaosAssertState.Reset();
        object value = new CastTypeCheckDerivedBox { BaseValue = 7, MiddleValue = 21, DerivedValue = 42 };
        var interfaceMatched = value as ICastTypeCheckView;
        var interfaceCasted = (ICastTypeCheckView)value;
        var middle = value as CastTypeCheckMiddleBox;
        var matched = value as CastTypeCheckBaseBox;
        var casted = (CastTypeCheckBaseBox)value;
        Assert.Equal(1, interfaceMatched is null ? 0 : 1);
        Assert.Equal(1, interfaceCasted is null ? 0 : 1);
        Assert.Equal(1, middle is null ? 0 : 1);
        Assert.Equal(1, matched is null ? 0 : 1);
        Assert.Equal(7, casted.BaseValue);
        Assert.Equal(21, ((CastTypeCheckMiddleBox)casted).MiddleValue);
        Assert.Equal(42, ((CastTypeCheckDerivedBox)casted).DerivedValue);
        return ChaosAssertState.Complete();
    }
}
