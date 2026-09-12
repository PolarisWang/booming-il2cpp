using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator.Tests.Infra;

/// <summary>
/// Provides a shared NativeAotLoweringPlanner instance and helper methods
/// for running the planner against in-memory artifact models.
/// </summary>
internal sealed class PlannerFixture
{
    private readonly NativeAotLoweringPlanner _planner = new();

    /// <summary>
    /// Path to the stub assembly DLL used as the closure input assembly.
    /// Built as part of the test project; provides valid PE metadata for
    /// the Loader stage called internally by the planner.
    /// </summary>
    internal static string StubAssemblyPath { get; } = LocateStubAssemblyDll();

    private static string LocateStubAssemblyDll()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();

        // Derive configuration from test output path, not hardcoded "Debug".
        // This makes tests work under any --configuration (Release, RelWithDebInfo, etc.).
        var testAsmDir = AppDomain.CurrentDomain.BaseDirectory;
        var configDir = "Debug";
        var knownConfigs = new[] { "Debug", "Release", "RelWithDebInfo", "Check" };
        foreach (var kc in knownConfigs)
        {
            if (testAsmDir.Contains($"/bin/{kc}/") || testAsmDir.Contains($"\\bin\\{kc}\\"))
            { configDir = kc; break; }
        }

        return Path.GetFullPath(Path.Combine(
            repoRoot,
            "tests", "managed", "Chaos.IL2CPP.CodeGen.Tests",
            "StubAssembly", "bin", configDir, "net8.0",
            "StubAssembly.dll"));
    }

    /// <summary>
    /// Runs the planner with the given artifacts and returns the template model.
    /// Uses minimal defaults for metadata/manifest artifacts.
    /// In full assembly mode the planner still requires a non-null entry method
    /// (it reads entryMethod.NativeSymbol), so we use the first method from
    /// the IR artifact.
    /// </summary>
    public NativeAotTemplateModel RunPlanner(
        AotCoreIrArtifact aotCoreIr,
        NativeAotLoweringPlanArtifact? loweringPlan = null,
        ManagedClosureManifestArtifact? closureManifest = null,
        MetadataRegistrationArtifact? metadataRegistration = null,
        SupplementalMetadataTemplateArtifact? supplementalMetadata = null)
    {
        loweringPlan ??= ModelFactory.CreateDefaultPlan();
        closureManifest ??= ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: StubAssemblyPath);
        metadataRegistration ??= ModelFactory.CreateEmptyMetadataRegistration();
        supplementalMetadata ??= ModelFactory.CreateEmptySupplementalMetadata();

        // In full assembly mode the planner skips entry validation but still
        // dereferences entryMethod.NativeSymbol. Pick the first IR method as
        // a safe sentinel — the planner processes all methods independently.
        var entryMethod = aotCoreIr.Methods.Count > 0
            ? aotCoreIr.Methods[0]
            : ModelFactory.CreateMethod("StubAssembly/StubMethods::ReturnZero:System.Int32()");

        return _planner.Create(
            loweringPlan,
            aotCoreIr,
            entryMethod,
            closureManifest,
            metadataRegistration,
            supplementalMetadata,
            fullAssemblyMode: true);
    }

    /// <summary>
    /// Runs the planner with a single method and returns its generated C++ source.
    /// Shortcut for the common case of testing one method's codegen output.
    /// </summary>
    public string RunPlannerSingleMethod(AotCoreIrMethodArtifact method)
    {
        var artifact = ModelFactory.CreateArtifact(method);
        var loweringPlan = ModelFactory.CreateDefaultPlan(method.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: StubAssemblyPath);

        // In full assembly mode, the entrySubjectId does not have to match
        // exactly, but we also need the plan's EntrySubjectId to correspond
        // to a method that exists in aotCoreIr.Methods. Use the method's own
        // subjectId to be safe.
        var templateModel = RunPlanner(artifact, loweringPlan, manifest);

        var matchMethod = templateModel.Methods.FirstOrDefault(m =>
            m.SubjectId == method.SubjectId);
        return matchMethod?.MethodSource ?? "<method not found in output>";
    }
}
