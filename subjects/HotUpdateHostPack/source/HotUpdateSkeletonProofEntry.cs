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
        Evidence = ChaosEvidenceKind.Stdout | ChaosEvidenceKind.Metadata,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"hot-update-skeleton-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            Console.WriteLine("HotUpdateSkeletonProof entry reached.");
            Console.WriteLine("args=0");
            Console.WriteLine($"before-load={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");

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

            Console.WriteLine($"hot-update-skeleton-load={loaded.ToString().ToLowerInvariant()}");
            Console.WriteLine($"after-load={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");
            Console.WriteLine($"hot-update-skeleton-dispatch={runtimeManager.DispatchInt32(HotUpdateAddSubjectId, GetAotAddFallback)}");
            Console.WriteLine($"hot-update-skeleton-mode={runtimeManager.Mode.ToString().ToLowerInvariant()}");
            Console.WriteLine($"integrity-after-load={runtimeManager.ValidateIntegrity().IsValid.ToString().ToLowerInvariant()}");
            Console.WriteLine($"active-patches-after-load={runtimeManager.GetActivePatches().Count}");
            runtimeManager.Rollback();
            Console.WriteLine($"after-rollback={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");
            Console.WriteLine($"hot-update-skeleton-after-rollback={runtimeManager.DispatchInt32(HotUpdateAddSubjectId, GetAotAddFallback)}");
            Console.WriteLine($"active-patches-after-rollback={runtimeManager.GetActivePatches().Count}");
            Console.WriteLine($"integrity-after-rollback={runtimeManager.ValidateIntegrity().IsValid.ToString().ToLowerInvariant()}");

            runtimeManager.LoadPackage(
                validPackageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [HotUpdateSubjectId] = 42,
                    [HotUpdateAddSubjectId] = 7,
                });
            Console.WriteLine($"after-reapply={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");
            runtimeManager.UnloadPackage();
            Console.WriteLine($"after-unload={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");

            try
            {
                var corruptPackageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                    workspace,
                    "corrupt",
                    HotUpdatePackageSupport.CurrentAotVersion,
                    "HotPatch/Patch::Apply()",
                    [0x48, 0x4F, 0x54, 0x34, 0x32],
                    "signed-proof",
                    corruptAssemblyHash: true);
                PackageReader.ReadFromDirectory(corruptPackageRoot);
                Console.WriteLine("corruption=unexpected-pass");
                return 1;
            }
            catch (InvalidDataException)
            {
                Console.WriteLine("corruption=rejected");
            }

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
