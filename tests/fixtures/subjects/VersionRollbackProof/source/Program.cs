using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;

namespace VersionRollbackProof;

internal static class Program
{
    private const string CurrentAotVersion = "1.0.0";
    private const string HotPatchValueAuthorityKey = "hot-update://version-rollback/value";
    private static readonly ManagedMethodIdentityArtifact HotPatchValueIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "VersionRollbackProof",
                DeclaringTypeSubjectId = "VersionRollbackProof/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "GetValue",
                SubjectId = "VersionRollbackProof/HotPatch::GetValue()",
                Signature = "System.Int32 HotPatch::GetValue()",
                ExecutionAuthorityKey = HotPatchValueAuthorityKey,
            });
    private static readonly ManagedMethodIdentityArtifact HotPatchValueBindingIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "VersionRollbackProof",
                DeclaringTypeSubjectId = "VersionRollbackProof/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "GetValue",
                SubjectId = "VersionRollbackProof/HotPatch::GetValue()",
                Signature = "System.Int32 HotPatch::GetValue()",
            });

    private static int Main(string[] args)
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"version-rollback-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            var v1Root = CreatePackageRoot(workspace, "v1", "1.0.0");
            var v2Root = CreatePackageRoot(workspace, "v2", "1.0.0");
            var incompatibleTargetAotRoot = CreatePackageRoot(workspace, "v3-target-aot", "2.0.0");
            var incompatibleKernelRoot = CreatePackageRoot(workspace, "v4-kernel", "1.0.0", kernelArtifactVersion: "v2");

            runtimeManager.LoadPackage(
                v1Root,
                CurrentAotVersion,
                CreateBindings(11));
            Console.WriteLine($"version-rollback-v1={runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback)}");
            var v1Handle = runtimeManager.CreateHandle(HotPatchValueIdentity);
            Console.WriteLine($"version-rollback-v1-handle={DispatchHandle(runtimeManager, v1Handle)}");

            runtimeManager.LoadPackage(
                v2Root,
                CurrentAotVersion,
                CreateBindings(22));
            Console.WriteLine($"version-rollback-v1-stale={RequireStaleHandle(runtimeManager, v1Handle)}");
            Console.WriteLine($"version-rollback-v2={runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback)}");
            var v2Handle = runtimeManager.CreateHandle(HotPatchValueIdentity);
            Console.WriteLine($"version-rollback-v2-handle={DispatchHandle(runtimeManager, v2Handle)}");

            runtimeManager.Rollback();
            Console.WriteLine($"version-rollback-v2-stale={RequireStaleHandle(runtimeManager, v2Handle)}");
            Console.WriteLine($"version-rollback-back-v1={runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback)}");

            runtimeManager.Rollback();
            Console.WriteLine($"version-rollback-back-aot={runtimeManager.DispatchInt32(HotPatchValueIdentity, GetAotFallback)}");

            var targetAotCompatible = runtimeManager.LoadPackage(
                incompatibleTargetAotRoot,
                CurrentAotVersion,
                CreateBindings(99));
            Console.WriteLine($"version-rollback-target-aot={(targetAotCompatible ? "unexpected" : "rejected")}");
            Console.WriteLine($"version-rollback-target-aot-reason={runtimeManager.LastError}");
            Console.WriteLine($"version-rollback-target-aot-can-rollback={FormatBool(runtimeManager.CanRollback)}");

            var kernelCompatible = runtimeManager.LoadPackage(
                incompatibleKernelRoot,
                CurrentAotVersion,
                CreateBindings(101));
            Console.WriteLine($"version-rollback-kernel-version={(kernelCompatible ? "unexpected" : "rejected")}");
            Console.WriteLine($"version-rollback-kernel-version-reason={runtimeManager.LastError}");
            Console.WriteLine($"version-rollback-kernel-version-can-rollback={FormatBool(runtimeManager.CanRollback)}");
            return targetAotCompatible || kernelCompatible ? 1 : 0;
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

    private static int DispatchHandle(RuntimeManager runtimeManager, HotUpdateMethodHandle handle)
    {
        if (!runtimeManager.TryDispatchHandle(handle, Array.Empty<object?>(), out var result, out var reasonCode))
        {
            throw new InvalidOperationException($"expected handle dispatch to succeed, reason={reasonCode}");
        }

        return Convert.ToInt32(result);
    }

    private static string RequireStaleHandle(RuntimeManager runtimeManager, HotUpdateMethodHandle handle)
    {
        if (runtimeManager.TryDispatchHandle(handle, Array.Empty<object?>(), out _, out var reasonCode))
        {
            throw new InvalidOperationException("expected stale handle dispatch to fail.");
        }

        if (!string.Equals(reasonCode, HotUpdateDispatchReasonCodes.StaleHandle, StringComparison.Ordinal))
        {
            throw new InvalidOperationException($"expected reason {HotUpdateDispatchReasonCodes.StaleHandle}, got {reasonCode}");
        }

        return reasonCode;
    }

    private static string FormatBool(bool value)
    {
        return value ? "true" : "false";
    }

    private static HotUpdateMethodBindingSet CreateBindings(int value)
    {
        return new HotUpdateMethodBindingSet
        {
            ConstantInt32Bindings =
            [
                new HotUpdateConstantInt32Binding
                {
                    Identity = HotPatchValueBindingIdentity,
                    ExecutionAuthorityKey = HotPatchValueAuthorityKey,
                    ConstantValue = value,
                },
            ],
        };
    }

    private static string CreatePackageRoot(
        string workspaceRoot,
        string suffix,
        string targetAotVersion,
        string kernelArtifactVersion = HotUpdateVersionContract.CurrentKernelArtifactVersion)
    {
        var packageRoot = Path.Combine(workspaceRoot, suffix);
        Directory.CreateDirectory(packageRoot);

        var assemblyBytes = new byte[] { 0x56, 0x52, 0x50, 0x31 };
        File.WriteAllBytes(Path.Combine(packageRoot, "HotPatch.dll"), assemblyBytes);
        File.WriteAllText(
            Path.Combine(packageRoot, "metadata-supplement.bin"),
            """
            {
              "Types": [],
              "Methods": [],
              "GenericInstantiations": []
            }
            """);

        var manifest = new HotUpdatePackage
        {
            PackageId = $"com.example.rollback.{suffix}",
            TargetAotVersion = targetAotVersion,
            PackageFormatVersion = HotUpdateVersionContract.CurrentPackageFormatVersion,
            KernelArtifactVersion = kernelArtifactVersion,
            Assemblies =
            [
                new HotUpdateAssemblyEntry
                {
                    Name = "HotPatch.dll",
                    Hash = PackageReader.ComputeFileHash(assemblyBytes),
                    Size = assemblyBytes.Length,
                    EntryPoint = "VersionRollbackProof/HotPatch::Apply()",
                },
            ],
            SupplementalMetadata = "metadata-supplement.bin",
            Signature = "rollback-proof",
        };

        File.WriteAllText(
            Path.Combine(packageRoot, PackageReader.ManifestFileName),
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return packageRoot;
    }
}
