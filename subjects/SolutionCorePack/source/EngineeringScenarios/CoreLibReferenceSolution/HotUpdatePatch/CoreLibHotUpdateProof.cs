using System;
using System.Collections.Generic;
using System.Linq;
using Chaos.TestFramework;

namespace GoldenCoreLibReference.HotUpdatePatch;

public static class CoreLibHotUpdateProof
{
    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "corelib-reference-hotupdate-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.PackageLoad,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.CoreLibReferenceSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.PackageLoad,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 1)]
    public static int Run()
    {
        var values = new List<int> { 3, 5, 8 };
        var sum = values.Sum();
        Console.WriteLine($"corelib-reference-hotupdate:System.Private.CoreLib|System.Runtime|System.Console:{sum}:{values.Count}");
        Assert.Equal(16, sum);
        Assert.Equal(3, values.Count);
        return 0;
    }
}
