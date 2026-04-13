using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class HotUpdateSkeletonProofEntry
{
    private const string HotUpdateSubjectId = "HotUpdateSkeletonProof/Helper::GetValue()";
    private const string HotUpdateAddSubjectId = "HotUpdateSkeletonProof/HotPatch::Add()";

    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "hot-update-skeleton-proof",
        Requires = ChaosRuntimeFeature.HotUpdate,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"hot-update-skeleton-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            var beforeLoad = runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue);
            Assert.Equal(1, beforeLoad);

            var validPackageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "valid",
                HotUpdatePackageSupport.CurrentAotVersion,
                "HotPatch/Patch::Apply()",
                [0x48, 0x4F, 0x54, 0x34, 0x32],
                "signed-proof");
            var loaded = runtimeManager.LoadPackage(
                validPackageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [HotUpdateSubjectId] = 42,
                    [HotUpdateAddSubjectId] = 7,
                });
            Assert.True(loaded);

            var afterLoad = runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue);
            var dispatchValue = runtimeManager.DispatchInt32(HotUpdateAddSubjectId, GetAotAddFallback);
            var integrityAfterLoad = runtimeManager.ValidateIntegrity();
            var activePatchesAfterLoad = runtimeManager.GetActivePatches().Count;
            Assert.Equal(42, afterLoad);
            Assert.Equal(7, dispatchValue);
            Assert.Equal(RuntimeMode.Mixed, runtimeManager.Mode);
            Assert.True(integrityAfterLoad.IsValid);
            Assert.Equal(1, activePatchesAfterLoad);
            runtimeManager.Rollback();
            var afterRollback = runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue);
            var afterRollbackDispatch = runtimeManager.DispatchInt32(HotUpdateAddSubjectId, GetAotAddFallback);
            var activePatchesAfterRollback = runtimeManager.GetActivePatches().Count;
            var integrityAfterRollback = runtimeManager.ValidateIntegrity();
            Assert.Equal(1, afterRollback);
            Assert.Equal(3, afterRollbackDispatch);
            Assert.Equal(0, activePatchesAfterRollback);
            Assert.True(integrityAfterRollback.IsValid);

            runtimeManager.LoadPackage(
                validPackageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [HotUpdateSubjectId] = 42,
                    [HotUpdateAddSubjectId] = 7,
                });
            var afterReapply = runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue);
            Assert.Equal(42, afterReapply);
            runtimeManager.UnloadPackage();
            var afterUnload = runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue);
            Assert.Equal(1, afterUnload);

            var corruptPackageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "corrupt",
                HotUpdatePackageSupport.CurrentAotVersion,
                "HotPatch/Patch::Apply()",
                [0x48, 0x4F, 0x54, 0x34, 0x32],
                "signed-proof",
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

    private static int GetAotAddFallback()
    {
        return 3;
    }

    private static class Helper
    {
        public static int GetValue()
        {
            return 1;
        }
    }
}
