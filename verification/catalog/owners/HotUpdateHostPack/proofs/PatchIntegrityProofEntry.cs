using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class PatchIntegrityProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "patch-integrity-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.PatchIntegrity,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.SkeletonPatchSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.PatchIntegrity,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"patch-integrity-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var validPackageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "valid",
                HotUpdatePackageSupport.CurrentAotVersion,
                "PatchIntegrityProof/HotPatch::Apply()",
                [0x50, 0x41, 0x54, 0x48],
                "integrity-proof");
            var validPackage = PackageReader.ReadFromDirectory(validPackageRoot);
            Assert.Equal("integrity-proof", validPackage.Manifest.Signature);

            var runtimeManager = new RuntimeManager();
            var loaded = runtimeManager.LoadPackage(
                validPackageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                new HotUpdateMethodBindingSet());
            Assert.True(loaded);
            Assert.True(runtimeManager.ValidateIntegrity().IsValid);

            var emptySignatureRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "empty-signature",
                HotUpdatePackageSupport.CurrentAotVersion,
                "PatchIntegrityProof/HotPatch::Apply()",
                [0x53, 0x49, 0x47, 0x30],
                string.Empty);
            var missingSignatureLoaded = runtimeManager.LoadPackage(
                emptySignatureRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                new HotUpdateMethodBindingSet());
            Assert.False(missingSignatureLoaded);
            Assert.NotNull(runtimeManager.LastError);

            var corruptPackageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "corrupt-hash",
                HotUpdatePackageSupport.CurrentAotVersion,
                "PatchIntegrityProof/HotPatch::Apply()",
                [0x43, 0x4F, 0x52, 0x52],
                "integrity-proof",
                corruptAssemblyHash: true);
            Assert.Throws<InvalidDataException>(() => PackageReader.ReadFromDirectory(corruptPackageRoot));
            return 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }
}
