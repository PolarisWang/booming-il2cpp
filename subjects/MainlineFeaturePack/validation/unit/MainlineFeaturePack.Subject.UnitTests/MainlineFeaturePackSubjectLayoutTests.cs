using System.Linq;
using System.Text.Json;
using Xunit;

namespace MainlineFeaturePack.Subject.UnitTests;

public sealed class MainlineFeaturePackSubjectLayoutTests
{
    [Fact]
    public void ManifestSeparatesProofEntryAndValidationSurfaces()
    {
        using var document = JsonDocument.Parse(
            RepoFiles.ReadText("subjects", "MainlineFeaturePack", "subject.manifest.json")
        );
        var root = document.RootElement;

        Assert.Equal("MainlineFeaturePack", root.GetProperty("subjectId").GetString());
        Assert.Equal("MainlineFeaturePack/ProofEntry::Run()", root.GetProperty("source").GetProperty("entry").GetString());

        var proofDevProfile = root.GetProperty("validationProfiles").GetProperty("proof-dev")
            .EnumerateArray()
            .Select(item => item.GetString())
            .ToArray();
        Assert.Contains("proof", proofDevProfile);
        Assert.Contains("unit", proofDevProfile);

        var unitValidation = root.GetProperty("validation").GetProperty("unit");
        Assert.Equal("xunit", unitValidation.GetProperty("framework").GetString());
        Assert.Equal(
            "subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj",
            unitValidation.GetProperty("project").GetString()
        );
    }

    [Fact]
    public void SourceTreeKeepsRuntimeEntrySeparateFromProofEntry()
    {
        var programText = RepoFiles.ReadText("subjects", "MainlineFeaturePack", "source", "Program.cs");

        Assert.Contains("internal static class ProofEntry", programText);
        Assert.Contains("public static int Run()", programText);
        Assert.Contains("Console.WriteLine(banner.BuildMessage());", programText);
        Assert.Contains("private static int Main(string[] args)", programText);
        Assert.Contains("File.WriteAllText(args[3], TraceDocumentFactory.GetWindowsWarmupTrace());", programText);
    }

    [Fact]
    public void ProofHostIsSubjectOwnedAndUsesMainlineFeaturePackImageName()
    {
        var mainCppText = RepoFiles.ReadText(
            "subjects",
            "MainlineFeaturePack",
            "validation",
            "proof",
            "native-reference",
            "main.cpp"
        );

        Assert.Contains("options.image_name_utf8 = \"MainlineFeaturePack\";", mainCppText);
        Assert.DoesNotContain("HelloWorldObject", mainCppText);
    }
}
