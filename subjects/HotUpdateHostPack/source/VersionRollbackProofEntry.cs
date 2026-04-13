using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class VersionRollbackProofEntry
{
    private const string SubjectId = "VersionRollbackProof/HotPatch::GetValue()";

    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "version-rollback-proof",
        Requires = ChaosRuntimeFeature.HotUpdate,
        Evidence = ChaosEvidenceKind.Stdout | ChaosEvidenceKind.Metadata,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"version-rollback-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            var v1Root = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v1",
                HotUpdatePackageSupport.CurrentAotVersion,
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x31],
                "rollback-proof");
            var v2Root = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v2",
                HotUpdatePackageSupport.CurrentAotVersion,
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x32],
                "rollback-proof");
            var incompatibleRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "v3",
                "2.0.0",
                "VersionRollbackProof/HotPatch::Apply()",
                [0x56, 0x52, 0x50, 0x33],
                "rollback-proof");

            runtimeManager.LoadPackage(
                v1Root,
                HotUpdatePackageSupport.CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 11,
                });
            Console.WriteLine($"version-rollback-v1={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            runtimeManager.LoadPackage(
                v2Root,
                HotUpdatePackageSupport.CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 22,
                });
            Console.WriteLine($"version-rollback-v2={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            runtimeManager.Rollback();
            Console.WriteLine($"version-rollback-back-v1={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            runtimeManager.Rollback();
            Console.WriteLine($"version-rollback-back-aot={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            var compatible = runtimeManager.LoadPackage(
                incompatibleRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 99,
                });
            Console.WriteLine($"version-rollback-compatibility={(compatible ? "unexpected" : "rejected")}");
            return compatible ? 1 : 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }

    private static int GetAotFallback()
    {
        return 5;
    }
}
