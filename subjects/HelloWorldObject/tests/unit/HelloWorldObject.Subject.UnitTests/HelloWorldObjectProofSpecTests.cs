using System;
using System.Linq;
using System.Text.Json;
using Xunit;

namespace HelloWorldObject.Subject.UnitTests;

public sealed class HelloWorldObjectProofSpecTests
{
    [Fact]
    public void SubjectProgramUsesCanonicalMainSignatureAndWorldConstant()
    {
        var programText = RepoFiles.ReadText("subjects", "HelloWorldObject", "source", "Program.cs");

        Assert.Contains("private static int Main(string[] args)", programText);
        Assert.Contains("new Greeter(\"World\")", programText);
        Assert.Contains("return 0;", programText);
        Assert.DoesNotContain("\"object-world\"", programText);
    }

    [Fact]
    public void SubjectManifestUsesSubjectRootMetadata()
    {
        using var document = JsonDocument.Parse(
            RepoFiles.ReadText("subjects", "HelloWorldObject", "subject.manifest.json")
        );
        var root = document.RootElement;

        Assert.Equal("HelloWorldObject", root.GetProperty("subjectId").GetString());
        Assert.Equal("HelloWorldObject", root.GetProperty("displayName").GetString());
        Assert.Equal("correctness.dev", root.GetProperty("defaultGoal").GetString());
        Assert.Equal("windows-dev-output", root.GetProperty("defaultMatrix").GetString());

        var source = root.GetProperty("source");
        Assert.Equal("dotnet-project", source.GetProperty("type").GetString());
        Assert.Equal(
            "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
            source.GetProperty("path").GetString()
        );
        Assert.Equal(
            "HelloWorldObject/Program::Main(System.String[])",
            source.GetProperty("entry").GetString()
        );
    }

    [Fact]
    public void ProofSpecDocUsesSubjectRootEntryPoints()
    {
        var specText = RepoFiles.ReadText("docs", "architecture", "roadmap-0", "hello-world-proof-spec-v0.md");
        var overviewText = RepoFiles.ReadText("contracts", "docs", "v0", "overview.md");

        Assert.Contains("subjects/HelloWorldObject/source/Program.cs", specText);
        Assert.Contains("subjects/HelloWorldObject/subject.manifest.json", specText);
        Assert.DoesNotContain("tests/proof/hello-world-object/suite.manifest.json", specText);
        Assert.Contains("docs/architecture/roadmap-0/hello-world-proof-spec-v0.md", overviewText);
    }

    [Fact]
    public void TypedIlExampleKeepsTheExclamationInBuildMessage()
    {
        using var document = JsonDocument.Parse(
            RepoFiles.ReadText("contracts", "artifacts", "v0", "samples", "typed-il-ir.min.json")
        );

        var buildMessage = document.RootElement
            .GetProperty("methods")
            .EnumerateArray()
            .Single(
                method => method.GetProperty("subjectId").GetString()
                    == "HelloWorldObject/Greeter::BuildMessage()"
            );
        var instructions = buildMessage
            .GetProperty("blocks")[0]
            .GetProperty("instructions")
            .EnumerateArray()
            .ToArray();

        Assert.Contains(
            instructions,
            instruction =>
                instruction.GetProperty("op").GetString() == "ldstr"
                && instruction.GetProperty("operand").GetString() == "!"
                && instruction.GetProperty("resultType").GetString() == "System.String"
        );
        Assert.True(
            instructions.Count(
                instruction =>
                    instruction.TryGetProperty("callee", out var callee)
                    && callee.GetString()
                        == "System.Private.CoreLib/System.String::Concat(System.String,System.String)"
            ) >= 2
        );
    }

}
