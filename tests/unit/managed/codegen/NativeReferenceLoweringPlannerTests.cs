using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class NativeReferenceLoweringPlannerTests
{
    private static LinkedWorldModel CreateLinkedWorld(string assemblyName, string entryPointSubjectId)
    {
        return new LinkedWorldModel
        {
            InputAssemblyPath = "test.dll",
            FullAssemblyClosure = false,
            Assembly = new AssemblyIdentityModel
            {
                Name = assemblyName,
                ModuleVersionId = Guid.NewGuid(),
            },
            Assemblies = [],
            EntryPointSubjectId = entryPointSubjectId,
            Types = [],
            Fields = [],
            Properties = [],
            Methods = [],
            Dependencies = [],
            CanonicalSubjects = new CanonicalSubjectsModel { Subjects = [] },
            SemanticShapes = new SemanticShapesModel
            {
                Types = [],
                Fields = [],
                Properties = [],
                Methods = [],
            },
            CapabilityBundles = new CapabilityBundlesModel
            {
                World = new WorldCapabilityBundleModel { Capabilities = [] },
                Methods = [],
            },
            OptimizationFacts = new OptimizationFactsArtifact
            {
                ClosedWorldSpecializations = [],
                DispatchFacts = [],
                LayoutFacts = [],
                ExceptionFacts = [],
            },
            PreserveDescriptor = new PreserveDescriptorArtifact
            {
                Entries = [],
            },
        };
    }

    private static NativeReferenceLoweringPlanner CreatePlanner()
    {
        return new NativeReferenceLoweringPlanner();
    }

    [Fact]
    public void Create_EngineLogWriteEntry_ReturnsEngineLogWritePlan()
    {
        var planner = CreatePlanner();
        var linkedWorld = CreateLinkedWorld(
            "EngineLogWriteLite",
            "EngineLogWriteLite/EngineLogWriteEntry::Run()");
        var typedIl = new TypedIlIrArtifact { Methods = [] };
        var metadataRegistration = new MetadataRegistrationArtifact { Registrations = [] };
        var codeRegistration = new CodeRegistrationArtifact { Modules = [] };

        var result = planner.Create(linkedWorld, typedIl, metadataRegistration, codeRegistration);

        Assert.NotNull(result);
        Assert.Equal("engine.log-write.minimal", result.PlanKind);
        Assert.Equal("EngineLogWriteLite/EngineLogWriteEntry::Run()", result.EntrySubjectId);
    }

    [Fact]
    public void Create_EngineObjectHandleEntry_ReturnsEngineObjectHandlePlan()
    {
        var planner = CreatePlanner();
        var linkedWorld = CreateLinkedWorld(
            "EngineObjectHandleLite",
            "EngineObjectHandleLite/EngineObjectHandleEntry::Run()");
        var typedIl = new TypedIlIrArtifact { Methods = [] };
        var metadataRegistration = new MetadataRegistrationArtifact { Registrations = [] };
        var codeRegistration = new CodeRegistrationArtifact { Modules = [] };

        var result = planner.Create(linkedWorld, typedIl, metadataRegistration, codeRegistration);

        Assert.NotNull(result);
        Assert.Equal("engine.object-handle.roundtrip.minimal", result.PlanKind);
        Assert.Equal("EngineObjectHandleLite/EngineObjectHandleEntry::Run()", result.EntrySubjectId);
    }

    [Fact]
    public void Create_EngineLifecycleCallbackEntry_ReturnsLifecyclePlan()
    {
        var planner = CreatePlanner();
        var linkedWorld = CreateLinkedWorld(
            "EngineLifecycleCallbackLite",
            "EngineLifecycleCallbackLite/EngineLifecycleCallbackEntry::Run()");
        var typedIl = new TypedIlIrArtifact { Methods = [] };
        var metadataRegistration = new MetadataRegistrationArtifact { Registrations = [] };
        var codeRegistration = new CodeRegistrationArtifact { Modules = [] };

        var result = planner.Create(linkedWorld, typedIl, metadataRegistration, codeRegistration);

        Assert.NotNull(result);
        Assert.Equal("engine.lifecycle-callback.minimal", result.PlanKind);
        Assert.Equal("EngineLifecycleCallbackLite/EngineLifecycleCallbackEntry::Run()", result.EntrySubjectId);
    }

    [Fact]
    public void Create_EngineHostProofEntry_ReturnsHostProofPlan()
    {
        var planner = CreatePlanner();
        var linkedWorld = CreateLinkedWorld(
            "EngineHostProof",
            "EngineHostProof/EngineHostEntry::Run()");
        var typedIl = new TypedIlIrArtifact { Methods = [] };
        var metadataRegistration = new MetadataRegistrationArtifact { Registrations = [] };
        var codeRegistration = new CodeRegistrationArtifact { Modules = [] };

        var result = planner.Create(linkedWorld, typedIl, metadataRegistration, codeRegistration);

        Assert.NotNull(result);
        Assert.Equal("engine.host-proof.minimal", result.PlanKind);
        Assert.Equal("EngineHostProof/EngineHostEntry::Run()", result.EntrySubjectId);
    }
}
