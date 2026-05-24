using System;
using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;

/// <summary>
/// Native-AOT entry for GenericSupplement subjects DLL.
/// Tests the SupplementalMetadataWriter → SupplementalMetadataLoader roundtrip.
/// Each test method returns 0 on success, nonzero on failure.
/// </summary>
internal static class GenericSupplementNativeEntry
{
    // ── Shared test data ────────────────────────────────────────────────
    private const string AotListInt32SubjectId = "System.Collections/System.Collections.Generic.List<System.Int32>";
    private const string HotUpdateTypeSubjectId = "GenericSupplementProof/HotStringListAdapter";
    private const int AotListInt32Token = 0x02000001;

    public static int TestSupplementalMetadataRoundtrip()
    {
        try
        {
            var listStringSubjectId = ManagedNaming.CreateInstantiatedTypeSubjectId(
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
                RegisteredMethods = [],
                ReservedSlots = new SupplementalMetadataReservedSlots(),
            };

            var writer = new SupplementalMetadataWriter();
            var payloadBytes = writer.WriteToBytes(
                template,
                new HotUpdateMetadataRequirements
                {
                    ReferencedAotTypeSubjectIds = [AotListInt32SubjectId],
                    ReferencedAotMethodSubjectIds = [],
                    HotUpdateTypeSubjectIds = [HotUpdateTypeSubjectId],
                    GenericInstantiationSubjectIds = [listStringSubjectId],
                });

            var loader = new SupplementalMetadataLoader();
            var loadedMetadata = loader.LoadFromBytes(payloadBytes);

            if (loadedMetadata == null)
                return 1;

            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int TestTypeLookupBySubjectId()
    {
        try
        {
            var listStringSubjectId = ManagedNaming.CreateInstantiatedTypeSubjectId(
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
                RegisteredMethods = [],
                ReservedSlots = new SupplementalMetadataReservedSlots(),
            };

            var writer = new SupplementalMetadataWriter();
            var payloadBytes = writer.WriteToBytes(template, new HotUpdateMetadataRequirements
            {
                ReferencedAotTypeSubjectIds = [AotListInt32SubjectId],
                ReferencedAotMethodSubjectIds = [],
                HotUpdateTypeSubjectIds = [HotUpdateTypeSubjectId],
                GenericInstantiationSubjectIds = [listStringSubjectId],
            });

            var loader = new SupplementalMetadataLoader();
            var loadedMetadata = loader.LoadFromBytes(payloadBytes);

            if (!loadedMetadata.TryGetTypeBySubjectId(HotUpdateTypeSubjectId, out var hotUpdateType) || hotUpdateType is null)
                return 1;

            if (hotUpdateType.SubjectId != HotUpdateTypeSubjectId)
                return 1;

            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int TestTypeLookupByToken()
    {
        try
        {
            var listStringSubjectId = ManagedNaming.CreateInstantiatedTypeSubjectId(
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
                RegisteredMethods = [],
                ReservedSlots = new SupplementalMetadataReservedSlots(),
            };

            var payloadBytes = new SupplementalMetadataWriter().WriteToBytes(template, new HotUpdateMetadataRequirements
            {
                ReferencedAotTypeSubjectIds = [AotListInt32SubjectId],
                ReferencedAotMethodSubjectIds = [],
                HotUpdateTypeSubjectIds = [HotUpdateTypeSubjectId],
                GenericInstantiationSubjectIds = [listStringSubjectId],
            });

            var loadedMetadata = new SupplementalMetadataLoader().LoadFromBytes(payloadBytes);

            if (!loadedMetadata.TryGetTypeBySubjectId(HotUpdateTypeSubjectId, out var hotUpdateType) || hotUpdateType is null)
                return 1;

            if (!loadedMetadata.TryGetTypeByToken(hotUpdateType.MetadataToken, out var byToken) || byToken is null)
                return 1;

            if (byToken.SubjectId != HotUpdateTypeSubjectId)
                return 1;

            return 0;
        }
        catch
        {
            return 1;
        }
    }

    public static int TestGenericInstantiation()
    {
        try
        {
            var listStringSubjectId = ManagedNaming.CreateInstantiatedTypeSubjectId(
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
                RegisteredMethods = [],
                ReservedSlots = new SupplementalMetadataReservedSlots(),
            };

            var payloadBytes = new SupplementalMetadataWriter().WriteToBytes(template, new HotUpdateMetadataRequirements
            {
                ReferencedAotTypeSubjectIds = [AotListInt32SubjectId],
                ReferencedAotMethodSubjectIds = [],
                HotUpdateTypeSubjectIds = [HotUpdateTypeSubjectId],
                GenericInstantiationSubjectIds = [listStringSubjectId],
            });

            var loadedMetadata = new SupplementalMetadataLoader().LoadFromBytes(payloadBytes);

            if (!loadedMetadata.HasGenericInstantiation(listStringSubjectId))
                return 1;

            var values = new List<string> { "hot", "update" };
            values.Add("ok");
            if (values.Count != 3)
                return 1;

            return 0;
        }
        catch
        {
            return 1;
        }
    }
}
