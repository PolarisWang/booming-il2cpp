using System.Text;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;
using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class MetadataSupplementProofEntry
{
    private const string AotListInt32SubjectId = "System.Collections/System.Collections.Generic.List<System.Int32>";
    private const string AotListAddMethodSubjectId = "System.Collections/System.Collections.Generic.List<System.Int32>::Add(System.Int32)";
    private const string HotUpdateTypeSubjectId = "HotUpdateHostPack/MetadataSupplementPatch";
    private const string PackageSubjectId = "MetadataSupplementProof/HotPatch::Apply()";
    private const int AotListInt32Token = 0x02000001;
    private const int AotListAddMethodToken = 0x06000001;
    private static readonly ManagedMethodIdentityArtifact AotListAddMethodIdentity =
        ManagedMethodIdentityResolver.Create(
            AotListAddMethodSubjectId,
            "System.Void System.Collections.Generic.List<System.Int32>::Add(System.Int32)");
    private static readonly ManagedMethodIdentityArtifact PackageIdentity =
        ManagedMethodIdentityResolver.Create(
            PackageSubjectId,
            "System.Int32 HotPatch::Apply()");

    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "metadata-supplement-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.HotUpdateMetadataSupplement,
        Requires = ChaosRuntimeFeature.HotUpdate | ChaosRuntimeFeature.MetadataSupplement | ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.FullProjectHotUpdateSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.MetadataSupplement,
        Priority = 2)]
    public static int Run()
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"metadata-supplement-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var stringListSubjectId = ManagedNaming.CreateInstantiatedTypeSubjectId(
                "System.Collections/System.Collections.Generic.List`1",
                ["System.String"]);

            var template = new SupplementalMetadataTemplateArtifact
            {
                RegisteredTypes =
                [
                    new SupplementalMetadataTypeTemplateEntry
                    {
                        AssemblyName = "System.Collections",
                        SubjectId = AotListInt32SubjectId,
                        DefinitionSubjectId = "System.Collections/System.Collections.Generic.List`1",
                        MetadataToken = AotListInt32Token,
                    },
                ],
                RegisteredMethods =
                [
                    new SupplementalMetadataMethodTemplateEntry
                    {
                        AssemblyName = "System.Collections",
                        SubjectId = AotListAddMethodSubjectId,
                        DefinitionSubjectId = AotListAddMethodSubjectId,
                        DeclaringTypeSubjectId = AotListInt32SubjectId,
                        MetadataToken = AotListAddMethodToken,
                        ParameterCount = 1,
                    },
                ],
                ReservedSlots = new SupplementalMetadataReservedSlots
                {
                    TypeCount = 4,
                    MethodCount = 4,
                    GenericInstantiationCount = 2,
                },
            };

            var payloadBytes = new SupplementalMetadataWriter().WriteToBytes(
                template,
                new HotUpdateMetadataRequirements
                {
                    ReferencedAotTypeSubjectIds = [AotListInt32SubjectId],
                    ReferencedAotMethodSubjectIds = [AotListAddMethodSubjectId],
                    HotUpdateTypeSubjectIds = [HotUpdateTypeSubjectId],
                    GenericInstantiationSubjectIds = [stringListSubjectId],
                });

            var packageRoot = HotUpdatePackageSupport.CreatePackageRoot(
                workspace,
                "metadata",
                HotUpdatePackageSupport.CurrentAotVersion,
                PackageSubjectId,
                [0x4D, 0x45, 0x54, 0x41],
                "metadata-proof",
                metadataContent: Encoding.UTF8.GetString(payloadBytes));

            var runtimeManager = new RuntimeManager();
            var loaded = runtimeManager.LoadPackage(
                packageRoot,
                HotUpdatePackageSupport.CurrentAotVersion,
                new HotUpdateMethodBindingSet
                {
                    ConstantInt32Bindings =
                    [
                        new HotUpdateConstantInt32Binding
                        {
                            Identity = PackageIdentity,
                            ConstantValue = 1,
                        },
                    ],
                });

            Assert.True(loaded);
            var integrity = runtimeManager.ValidateIntegrity();
            Assert.True(integrity.IsValid);
            Assert.Equal(1, runtimeManager.GetActivePatches().Count);

            Assert.NotNull(runtimeManager.SupplementalMetadata.ActiveMetadata);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetMethod(PackageIdentity, out var packageMethod));
            Assert.NotNull(packageMethod);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetTypeBySubjectId(HotUpdateTypeSubjectId, out var hotUpdateType));
            Assert.NotNull(hotUpdateType);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetTypeByToken(hotUpdateType!.MetadataToken, out var hotUpdateTypeByToken));
            Assert.NotNull(hotUpdateTypeByToken);
            Assert.Equal(HotUpdateTypeSubjectId, hotUpdateTypeByToken!.SubjectId);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetTypeBySubjectId(AotListInt32SubjectId, out _));
            Assert.True(runtimeManager.SupplementalMetadata.TryGetMethod(AotListAddMethodIdentity, out var aotMethod));
            Assert.NotNull(aotMethod);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetMethodByToken(AotListAddMethodToken, out var aotMethodByToken));
            Assert.NotNull(aotMethodByToken);
            Assert.Equal(AotListAddMethodSubjectId, aotMethodByToken!.SubjectId);
            Assert.True(runtimeManager.SupplementalMetadata.HasGenericInstantiation(stringListSubjectId));

            var values = new List<string> { "hot", "update" };
            values.Add("ok");
            Assert.Equal(3, values.Count);

            runtimeManager.UnloadPackage();
            Assert.Equal(0, runtimeManager.GetActivePatches().Count);
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
