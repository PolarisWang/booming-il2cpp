using Xunit;

namespace HelloWorldObject.Subject.UnitTests;

public sealed class HelloWorldObjectWindowsCutoverTests
{
    [Fact]
    public void ProofHostCMakeConsumesSubjectGeneratedBucketWithoutHiddenStage4Codegen()
    {
        var proofHostCmakeText = RepoFiles.ReadText(
            "subjects",
            "HelloWorldObject",
            "validation",
            "proof",
            "native-reference",
            "CMakeLists.txt"
        );

        var requiredMarkers = new[]
        {
            "artifacts/subjects/HelloWorldObject/runs/subject-exec/analysis/generated",
            "native-reference.generated.cpp",
            "chaos_subject_reference_proof",
            "chaos_subject_reference_proof_run",
            "stdout.log",
            "stderr.log",
            "exit-code.txt",
            "CHAOS_SUBJECT_GENERATED_ROOT",
            "CHAOS_SUBJECT_BUILD_OUT_ROOT",
            "CHAOS_SUBJECT_RUNTIME_ROOT",
            "CHAOS_SUBJECT_REFERENCE_PROOF_EXE",
            "CHAOS_VARIANT_CHECK",
            "CHAOS_VARIANT_PROFILE",
            "CHAOS_VARIANT_SHIP",
        };
        var forbiddenMarkers = new[]
        {
            "dotnet build",
            "emit-native-reference",
            "artifacts/proof/managed-closure/HelloWorldObject",
            "artifacts/proof/native-reference/HelloWorldObject",
            "tests/proof/input/HelloWorldObject/HelloWorldObject.csproj",
            "chaos_stage4_hello_world_object_proof",
            "chaos_stage4_hello_world_object_proof_run",
            "CHAOS_HELLOWORLD_GENERATED_ROOT",
            "CHAOS_HELLOWORLD_BUILD_OUT_ROOT",
            "CHAOS_HELLOWORLD_RUNTIME_ROOT",
            "CHAOS_STAGE4_",
        };

        foreach (var marker in requiredMarkers)
        {
            Assert.Contains(marker, proofHostCmakeText);
        }

        foreach (var marker in forbiddenMarkers)
        {
            Assert.DoesNotContain(marker, proofHostCmakeText);
        }
    }

    [Fact]
    public void LegacyVerifyRoadmap0PythonWrapperForwardsToRuntimeBaselineEntrypoint()
    {
        var verifyText = RepoFiles.ReadText("build", "scripts", "verify-roadmap-0.py");

        Assert.Contains("verify-runtime-baseline.py", verifyText);
        Assert.Contains("runpy.run_path", verifyText);
        Assert.DoesNotContain("subject_executor", verifyText);
        Assert.DoesNotContain("subject_planner", verifyText);
    }

    [Fact]
    public void VerifyRuntimeBaselineWindowsBranchRunsSubjectMatricesInsteadOfStage4CodegenChain()
    {
        var verifyText = RepoFiles.ReadText("build", "scripts", "verify-runtime-baseline.py");

        var requiredMarkers = new[]
        {
            "subject_executor",
            "subject_planner",
            "windows-dev-output",
            "windows-reference-trace",
            "windows-android-buildable",
            "windows-linux-buildable",
            "windows-stage4-native-reference.gate.json",
            "windows-reference-desktop.gate.json",
            "android-startup-smoke.gate.json",
            "linux-packaging.gate.json",
        };
        var forbiddenMarkers = new[]
        {
            "tests/proof/input/HelloWorldObject/bin/Release/net8.0/HelloWorldObject.dll",
            "artifacts/proof/managed-closure/HelloWorldObject",
            "artifacts/proof/native-reference/HelloWorldObject",
            "invoke_stage4_native_reference_codegen",
        };

        foreach (var marker in requiredMarkers)
        {
            Assert.Contains(marker, verifyText);
        }

        foreach (var marker in forbiddenMarkers)
        {
            Assert.DoesNotContain(marker, verifyText);
        }
    }

    [Fact]
    public void PowerShellVerifyWrapperForwardsToPythonEntrypoint()
    {
        var verifyText = RepoFiles.ReadText("build", "scripts", "verify-roadmap-0.ps1");

        Assert.Contains("verify-runtime-baseline.py", verifyText);
        Assert.Contains("--host-profile", verifyText);
        Assert.DoesNotContain("Invoke-Stage4NativeReferenceCodegen", verifyText);
    }

    [Fact]
    public void TestsProofTreeOnlyContainsGenericMigrationNote()
    {
        var noteText = RepoFiles.ReadText(
            "tests",
            "proof",
            "README.md"
        );

        Assert.Contains("subjects/<subject-id>/source/", noteText);
        Assert.Contains("subjects/<subject-id>/validation/proof/", noteText);
        Assert.False(RepoFiles.Exists("tests", "proof", "hello-world-object"));
        Assert.False(RepoFiles.Exists("tests", "proof", "input", "HelloWorldObject"));
        Assert.False(RepoFiles.Exists("tests", "proof", "input", "GenericEchoClosedMinimal"));
        Assert.False(RepoFiles.Exists("tests", "proof", "input", "ReflectionLiteQueryMinimal"));
        Assert.False(RepoFiles.Exists("tests", "proof", "input", "PInvokeLiteDllImportMinimal"));
        Assert.False(RepoFiles.Exists("tests", "proof", "native-reference", "HelloWorldObject"));
        Assert.False(RepoFiles.Exists("tests", "proof", "native-reference", "GenericEchoClosedMinimal"));
        Assert.False(RepoFiles.Exists("tests", "proof", "native-reference", "ReflectionLiteQueryMinimal"));
        Assert.False(RepoFiles.Exists("tests", "proof", "native-reference", "PInvokeLiteDllImportMinimal"));
    }

    [Fact]
    public void SubjectInputProjectDeclaresTargetFramework()
    {
        var projectText = RepoFiles.ReadText(
            "subjects",
            "HelloWorldObject",
            "source",
            "HelloWorldObject.csproj"
        );

        Assert.Contains("<TargetFramework>net8.0</TargetFramework>", projectText);
    }

    [Fact]
    public void SubjectProgramDeclaresConsoleNamespace()
    {
        var programText = RepoFiles.ReadText("subjects", "HelloWorldObject", "source", "Program.cs");

        Assert.Contains("using System;", programText);
    }
}
