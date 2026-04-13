using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.HotUpdate;

internal static class Program
{
    private const string AotListInt32SubjectId = "System.Collections/System.Collections.Generic.List<System.Int32>";
    private const string HotUpdateTypeSubjectId = "GenericSupplementProof/HotStringListAdapter";
    private const int AotListInt32Token = 0x02000001;
    private const string ExpectedGenericSupplementOutput = "generic-supplement=ok";

    private static int Main(string[] args)
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

        if (!loadedMetadata.TryGetTypeBySubjectId(HotUpdateTypeSubjectId, out var hotUpdateType) || hotUpdateType is null)
        {
            return 1;
        }

        if (!loadedMetadata.TryGetTypeByToken(hotUpdateType.MetadataToken, out var hotUpdateTypeByToken) || hotUpdateTypeByToken is null)
        {
            return 1;
        }

        if (!loadedMetadata.TryGetTypeBySubjectId(AotListInt32SubjectId, out _))
        {
            return 1;
        }

        if (!loadedMetadata.HasGenericInstantiation(listStringSubjectId))
        {
            return 1;
        }

        var values = new List<string> { "hot", "update" };
        values.Add("ok");
        if (values.Count != 3)
        {
            return 1;
        }

        Console.WriteLine("GenericSupplementProof entry reached.");
        Console.WriteLine($"args={args.Length}");
        Console.WriteLine($"generic-supplement={(hotUpdateTypeByToken.SubjectId == HotUpdateTypeSubjectId ? "ok" : "failed")}");
        return 0;
    }
}
