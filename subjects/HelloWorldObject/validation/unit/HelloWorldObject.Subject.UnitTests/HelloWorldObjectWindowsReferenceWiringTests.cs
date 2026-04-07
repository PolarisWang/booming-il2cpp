using Xunit;

namespace HelloWorldObject.Subject.UnitTests;

public sealed class HelloWorldObjectWindowsReferenceWiringTests
{
    [Fact]
    public void WindowsReferenceRouterKeepsContractSmokeAndAddsStage4NativeGraph()
    {
        var rootCmakeText = RepoFiles.ReadText("CMakeLists.txt");

        var requiredMarkers = new[]
        {
            "if(ROADMAP0_PRESET_TARGET STREQUAL \"windows-x64-reference\")",
            "add_subdirectory(src/native/runtime-core)",
            "add_subdirectory(src/native/support)",
            "add_subdirectory(src/native/bootstrap)",
            "add_subdirectory(subjects/HelloWorldObject/validation/proof/native-reference)",
            "add_subdirectory(tests/contracts/native/abi)",
        };
        var forbiddenMarkers = new[]
        {
            "add_subdirectory(tests/proof/native-reference/HelloWorldObject)",
            "add_subdirectory(tests/proof/native-reference/GenericEchoClosedMinimal)",
            "add_subdirectory(tests/proof/native-reference/ReflectionLiteQueryMinimal)",
        };

        foreach (var marker in requiredMarkers)
        {
            Assert.Contains(marker, rootCmakeText);
        }

        foreach (var marker in forbiddenMarkers)
        {
            Assert.DoesNotContain(marker, rootCmakeText);
        }
    }

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
            "add_custom_target(chaos_subject_reference_proof_run",
            "RunNativeReferenceProof.cmake",
            "stdout.log",
            "stderr.log",
            "exit-code.txt",
            "EXCLUDE_FROM_ALL",
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
            "artifacts/proof/managed-closure/HelloWorldObject",
            "artifacts/proof/native-reference/HelloWorldObject",
            "Chaos.IL2CPP.Driver.csproj",
            "tests/proof/input/HelloWorldObject/HelloWorldObject.csproj",
            "emit-native-reference",
            "Chaos.IL2CPP.Driver.dll",
            "add_custom_target(chaos_stage4_hello_world_object_host_input_build",
            "add_custom_target(chaos_stage4_hello_world_object_driver_build",
            "add_custom_target(chaos_stage4_hello_world_object_managed_closure",
            "add_custom_target(chaos_stage4_hello_world_object_codegen",
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
    public void StaticNativeTargetsDisableDllImportForRuntimeAbiHeaders()
    {
        var runtimeAbiHeaderText = RepoFiles.ReadText("contracts", "native", "v0", "runtime_abi.h");
        var runtimeCoreCmakeText = RepoFiles.ReadText("src", "native", "runtime-core", "CMakeLists.txt");
        var bootstrapCmakeText = RepoFiles.ReadText("src", "native", "bootstrap", "CMakeLists.txt");
        var supportCmakeText = RepoFiles.ReadText("src", "native", "support", "CMakeLists.txt");

        Assert.Contains("defined(CHAOS_RUNTIME_ABI_STATIC)", runtimeAbiHeaderText);
        Assert.Contains("#define CHAOS_RUNTIME_ABI_EXPORT", runtimeAbiHeaderText);
        Assert.Contains("PUBLIC CHAOS_RUNTIME_ABI_STATIC", runtimeCoreCmakeText);
        Assert.Contains("PUBLIC CHAOS_RUNTIME_ABI_STATIC", bootstrapCmakeText);
        Assert.Contains("PUBLIC CHAOS_RUNTIME_ABI_STATIC", supportCmakeText);
    }
}
