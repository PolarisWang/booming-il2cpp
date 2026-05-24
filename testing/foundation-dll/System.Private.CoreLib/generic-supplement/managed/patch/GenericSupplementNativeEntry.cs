using System;
using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;

/// <summary>
/// Hand-written native-AOT entry point for GenericSupplement family.
/// Tests the SupplementalMetadataWriter → SupplementalMetadataLoader roundtrip:
///   1. Create and write a metadata artifact
///   2. Load from bytes
///   3. Verify type lookups by subject ID and by metadata token
///   4. Verify generic instantiation tracking
/// Each test method returns 0 on success, nonzero on failure.
/// </summary>
internal static class GenericSupplementNativeEntry
{
    // ── Shared test data ────────────────────────────────────────────────
    private const string AotListInt32SubjectId = "System.Collections/System.Collections.Generic.List<System.Int32>";
    private const string HotUpdateTypeSubjectId = "GenericSupplementProof/HotStringListAdapter";
    private const int AotListInt32Token = 0x02000001;

    /// <summary>Dispatcher called from generated Subjects code.</summary>
    public static int Run(int entryIndex)
    {
        return entryIndex switch
        {
            0 => TestSupplementalMetadataRoundtrip(),
            1 => TestTypeLookupBySubjectId(),
            2 => TestTypeLookupByToken(),
            3 => TestGenericInstantiation(),
            _ => 1, // unknown entry
        };
    }

    /// <summary>Create a metadata artifact, write to bytes, load back.</summary>
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

            return 0; // roundtrip succeeded
        }
        catch
        {
            return 1;
        }
    }

    /// <summary>Look up a hot-update type by subject ID.</summary>
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

    /// <summary>Look up a type by metadata token.</summary>
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

    /// <summary>Verify generic instantiation tracking.</summary>
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
