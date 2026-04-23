using System.Reflection;
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
    private const string PackageExecutionAuthorityKey = "hot-update://metadata-supplement/apply";
    private static readonly ManagedMethodIdentityArtifact AotListAddMethodIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "System.Collections",
                DeclaringTypeSubjectId = AotListInt32SubjectId,
                DeclaringTypeDisplayName = "System.Collections.Generic.List<System.Int32>",
                MethodName = "Add",
                SubjectId = AotListAddMethodSubjectId,
                Signature = "System.Void System.Collections.Generic.List<System.Int32>::Add(System.Int32)",
            });
    private static readonly ManagedMethodIdentityArtifact PackageIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "MetadataSupplementProof",
                DeclaringTypeSubjectId = "MetadataSupplementProof/HotPatch",
                DeclaringTypeDisplayName = "HotPatch",
                MethodName = "Apply",
                SubjectId = PackageSubjectId,
                Signature = "System.Int32 HotPatch::Apply()",
            });
    private static readonly ManagedMethodIdentityArtifact PackageAuthorityIdentity =
        ManagedMethodIdentityResolver.Create(
            new ManagedMethodIdentitySpec
            {
                AssemblyName = "MetadataSupplementProof",
                DeclaringTypeSubjectId = "MetadataSupplementProof/AuthorityCarrier",
                DeclaringTypeDisplayName = "AuthorityCarrier",
                MethodName = "Apply",
                SubjectId = "MetadataSupplementProof/AuthorityCarrier::Apply()",
                Signature = "System.Int32 HotPatch::Apply()",
                ExecutionAuthorityKey = PackageExecutionAuthorityKey,
            });

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
            var intListType = typeof(List<int>);
            var constructor = intListType.GetConstructors(BindingFlags.Instance | BindingFlags.Public)[0];
            var addMethod = intListType.GetMethod(nameof(List<int>.Add), BindingFlags.Instance | BindingFlags.Public)!;
            var stringListType = typeof(List<string>);
            var stringConstructor = stringListType.GetConstructors(BindingFlags.Instance | BindingFlags.Public)[0];
            var stringAddMethod = stringListType.GetMethod(nameof(List<string>.Add), BindingFlags.Instance | BindingFlags.Public)!;
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
                        MetadataToken = intListType.MetadataToken,
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
                        MetadataToken = addMethod.MetadataToken,
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
                metadataContent: Encoding.UTF8.GetString(payloadBytes),
                executionAuthorityKey: PackageExecutionAuthorityKey);

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
            Assert.True(runtimeManager.SupplementalMetadata.TryGetMethodByExecutionAuthority(PackageAuthorityIdentity, out var packageMethodByAuthority));
            Assert.NotNull(packageMethodByAuthority);
            Assert.Equal(PackageSubjectId, packageMethodByAuthority!.SubjectId);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetTypeBySubjectId(HotUpdateTypeSubjectId, out var hotUpdateType));
            Assert.NotNull(hotUpdateType);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetTypeByToken(hotUpdateType!.MetadataToken, out var hotUpdateTypeByToken));
            Assert.NotNull(hotUpdateTypeByToken);
            Assert.Equal(HotUpdateTypeSubjectId, hotUpdateTypeByToken!.SubjectId);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetTypeBySubjectId(AotListInt32SubjectId, out var aotType));
            Assert.NotNull(aotType);
            Assert.Equal(intListType.MetadataToken, aotType!.MetadataToken);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetMethod(AotListAddMethodIdentity, out var aotMethod));
            Assert.NotNull(aotMethod);
            Assert.Equal(addMethod.MetadataToken, aotMethod!.MetadataToken);
            Assert.True(runtimeManager.SupplementalMetadata.TryGetMethodByToken(addMethod.MetadataToken, out var aotMethodByToken));
            Assert.NotNull(aotMethodByToken);
            Assert.Equal(AotListAddMethodSubjectId, aotMethodByToken!.SubjectId);
            Assert.True(runtimeManager.SupplementalMetadata.HasGenericInstantiation(stringListSubjectId));

            var constructedList = (List<int>)constructor.Invoke(parameters: null)!;
            addMethod.Invoke(constructedList, [21]);
            addMethod.Invoke(constructedList, [84]);
            Assert.Equal(2, constructedList.Count);
            Assert.Equal(21, constructedList[0]);
            Assert.Equal(84, constructedList[1]);

            var constructedStringList = (List<string>)stringConstructor.Invoke(parameters: null)!;
            stringAddMethod.Invoke(constructedStringList, ["bridge"]);
            Assert.Equal(1, constructedStringList.Count);
            Assert.Equal("bridge", constructedStringList[0]);

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
