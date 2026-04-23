from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
CODEGEN_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Chaos.IL2CPP.CodeGen.csproj"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase2-aot-core-ir-ldtoken-target-reference"
TYPE_FIXTURE_ASSEMBLY_NAME = "LdtokenTypeDecodeRepro"
TYPE_HELPER_SUBJECT_ID = f"{TYPE_FIXTURE_ASSEMBLY_NAME}/Program::TypeHandleRoundTrip()"
SYNTHETIC_ASSEMBLY_NAME = "SyntheticLdtokenHarness"
SYNTHETIC_MAIN_SUBJECT_ID = f"{SYNTHETIC_ASSEMBLY_NAME}/Program::Main()"


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))

class Phase2AotCoreIrLdtokenTargetReferenceTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.workspace_root = TEST_OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.type_output_root = TEST_OUTPUT_ROOT / f"type-bundle-{uuid.uuid4().hex}"
        cls.synthetic_output_path = TEST_OUTPUT_ROOT / f"synthetic-aot-core-ir-{uuid.uuid4().hex}.json"
        cls.type_bundle_generated = False
        cls.synthetic_output_generated = False

    def _write_type_fixture_project(self) -> Path:
        project_root = self.workspace_root / TYPE_FIXTURE_ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{TYPE_FIXTURE_ASSEMBLY_NAME}.csproj").write_text(
            """
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <ImplicitUsings>disable</ImplicitUsings>
    <Nullable>disable</Nullable>
  </PropertyGroup>
</Project>
""".strip()
            + "\n",
            encoding="utf-8",
        )
        (project_root / "Program.cs").write_text(
            """
using System;

internal static class Program
{
    private static int Main()
    {
        return TypeHandleRoundTrip();
    }

    internal static int TypeHandleRoundTrip()
    {
        RuntimeTypeHandle handle = typeof(TokenCarrier).TypeHandle;
        Type resolved = Type.GetTypeFromHandle(handle);
        return resolved == typeof(TokenCarrier) ? 0 : 1;
    }
}

internal sealed class TokenCarrier
{
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_type_bundle_generated(self) -> None:
        if self.__class__.type_bundle_generated:
            return

        if self.workspace_root.exists():
            shutil.rmtree(self.workspace_root)
        if self.type_output_root.exists():
            shutil.rmtree(self.type_output_root)

        project_root = self._write_type_fixture_project()
        project_path = project_root / f"{TYPE_FIXTURE_ASSEMBLY_NAME}.csproj"
        assembly_path = project_root / "bin" / "Release" / "net8.0" / f"{TYPE_FIXTURE_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(assembly_path.is_file(), msg=f"missing ldtoken type fixture assembly: {assembly_path}")
        run_checked(["dotnet", str(DRIVER_DLL_PATH), str(assembly_path), str(self.type_output_root)], cwd=REPO_ROOT)

        self.__class__.type_bundle_generated = True

    def _write_synthetic_harness_project(self) -> Path:
        project_root = self.workspace_root / "SyntheticLdtokenHarness"
        project_root.mkdir(parents=True, exist_ok=True)
        codegen_project_path = CODEGEN_PROJECT_PATH.as_posix()
        (project_root / "SyntheticLdtokenHarness.csproj").write_text(
            f"""
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <ImplicitUsings>enable</ImplicitUsings>
    <Nullable>disable</Nullable>
  </PropertyGroup>
  <ItemGroup>
    <ProjectReference Include="{codegen_project_path}" />
  </ItemGroup>
</Project>
""".strip()
            + "\n",
            encoding="utf-8",
        )
        (project_root / "Program.cs").write_text(
            """
using System.Text.Json;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;

var assembly = new AssemblyIdentityModel
{
    Name = "SyntheticLdtokenHarness",
    ModuleVersionId = Guid.Parse("11111111-2222-3333-4444-555555555555"),
};

const string assemblyName = "SyntheticLdtokenHarness";
const string programTypeSubjectId = "SyntheticLdtokenHarness/Program";
const string tokenHolderTypeSubjectId = "SyntheticLdtokenHarness/TokenHolder";
const string helperMethodSubjectId = "SyntheticLdtokenHarness/TokenHolder::Helper()";
const string mainMethodSubjectId = "SyntheticLdtokenHarness/Program::Main()";
const string fieldSubjectId = "SyntheticLdtokenHarness/TokenHolder::Value";

var programType = new ManagedTypeModel
{
    AssemblyName = assemblyName,
    NamespaceName = null,
    Name = "Program",
    SubjectId = programTypeSubjectId,
    DefinitionSubjectId = programTypeSubjectId,
    DisplayName = "Program",
    IsInterface = false,
    IsValueType = false,
    BaseTypeSubjectId = "System.Private.CoreLib/System.Object",
    ImplementedInterfaceSubjectIds = [],
    IsPreserved = true,
    MetadataToken = 0x02000001,
};

var tokenHolderType = new ManagedTypeModel
{
    AssemblyName = assemblyName,
    NamespaceName = null,
    Name = "TokenHolder",
    SubjectId = tokenHolderTypeSubjectId,
    DefinitionSubjectId = tokenHolderTypeSubjectId,
    DisplayName = "TokenHolder",
    IsInterface = false,
    IsValueType = false,
    BaseTypeSubjectId = "System.Private.CoreLib/System.Object",
    ImplementedInterfaceSubjectIds = [],
    IsPreserved = true,
    MetadataToken = 0x02000002,
};

var valueField = new ManagedFieldModel
{
    AssemblyName = assemblyName,
    DeclaringTypeSubjectId = tokenHolderTypeSubjectId,
    Name = "Value",
    FieldType = "System.Int32",
    SubjectId = fieldSubjectId,
    DefinitionSubjectId = fieldSubjectId,
    IsStatic = true,
    IsThreadStatic = false,
    IsPreserved = true,
    MetadataToken = 0x04000001,
};

var helperMethod = new ManagedMethodModel
{
    AssemblyName = assemblyName,
    DeclaringTypeSubjectId = tokenHolderTypeSubjectId,
    DeclaringTypeDisplayName = "TokenHolder",
    Name = "Helper",
    ReturnType = "System.Void",
    SubjectId = helperMethodSubjectId,
    DefinitionSubjectId = helperMethodSubjectId,
    Signature = "System.Void TokenHolder::Helper()",
    IsStatic = true,
    IsVirtual = false,
    IsPreserved = true,
    IsUnmanagedCallersOnly = false,
    MetadataToken = 0x06000001,
    Parameters = [],
    Body = new ManagedMethodBodyModel
    {
        Blocks =
        [
            new ManagedBlockModel
            {
                BlockId = "IL_0000",
                Instructions =
                [
                    new ManagedInstructionModel
                    {
                        Op = "ret",
                        IlOffset = 0,
                    },
                ],
            },
        ],
        ExceptionRegions = [],
    },
};

var mainMethod = new ManagedMethodModel
{
    AssemblyName = assemblyName,
    DeclaringTypeSubjectId = programTypeSubjectId,
    DeclaringTypeDisplayName = "Program",
    Name = "Main",
    ReturnType = "System.Void",
    SubjectId = mainMethodSubjectId,
    DefinitionSubjectId = mainMethodSubjectId,
    Signature = "System.Void Program::Main()",
    IsStatic = true,
    IsVirtual = false,
    IsPreserved = true,
    IsUnmanagedCallersOnly = false,
    MetadataToken = 0x06000002,
    Parameters = [],
    Body = new ManagedMethodBodyModel
    {
        Blocks =
        [
            new ManagedBlockModel
            {
                BlockId = "IL_0000",
                Instructions =
                [
                    new ManagedInstructionModel
                    {
                        Op = "ldtoken",
                        Operand = tokenHolderTypeSubjectId,
                        IlOffset = 0,
                        ResultType = "System.RuntimeTypeHandle",
                        Reference = new ManagedInstructionReference
                        {
                            AssemblyName = assemblyName,
                            SubjectKind = "type",
                            SubjectId = tokenHolderTypeSubjectId,
                        },
                    },
                    new ManagedInstructionModel
                    {
                        Op = "ldtoken",
                        Operand = fieldSubjectId,
                        IlOffset = 1,
                        ResultType = "System.RuntimeFieldHandle",
                        Reference = new ManagedInstructionReference
                        {
                            AssemblyName = assemblyName,
                            SubjectKind = "field",
                            SubjectId = fieldSubjectId,
                        },
                    },
                    new ManagedInstructionModel
                    {
                        Op = "ldtoken",
                        Operand = helperMethodSubjectId,
                        IlOffset = 2,
                        ResultType = "System.RuntimeMethodHandle",
                        Reference = new ManagedInstructionReference
                        {
                            AssemblyName = assemblyName,
                            SubjectKind = "method",
                            SubjectId = helperMethodSubjectId,
                        },
                    },
                    new ManagedInstructionModel
                    {
                        Op = "ret",
                        IlOffset = 3,
                    },
                ],
            },
        ],
        ExceptionRegions = [],
    },
};

var typedIl = new TypedIlIrArtifact
{
    Methods =
    [
        new TypedIlMethodArtifact
        {
            MethodId = ManagedNaming.CreateMethodId(helperMethod),
            SubjectId = helperMethod.SubjectId,
            Signature = helperMethod.Signature,
            Identity = ManagedMethodIdentityResolver.Create(helperMethod),
            MethodRole = "implementation",
            BodyAvailability = "nativeGenerated",
            BodyAvailabilityCode = BodyAvailabilityCode.NativeGenerated,
            Capabilities = [],
            Parameters = [],
            Blocks =
            [
                new TypedIlBlockArtifact
                {
                    BlockId = "IL_0000",
                    Instructions =
                    [
                        new TypedIlInstructionArtifact
                        {
                            Op = "ret",
                        },
                    ],
                },
            ],
        },
        new TypedIlMethodArtifact
        {
            MethodId = ManagedNaming.CreateMethodId(mainMethod),
            SubjectId = mainMethod.SubjectId,
            Signature = mainMethod.Signature,
            Identity = ManagedMethodIdentityResolver.Create(mainMethod),
            MethodRole = "managed-entry",
            BodyAvailability = "nativeGenerated",
            BodyAvailabilityCode = BodyAvailabilityCode.NativeGenerated,
            Capabilities = [],
            Parameters = [],
            Blocks =
            [
                new TypedIlBlockArtifact
                {
                    BlockId = "IL_0000",
                    Instructions =
                    [
                        new TypedIlInstructionArtifact
                        {
                            Op = "ldtoken",
                            Operand = tokenHolderTypeSubjectId,
                            ResultType = "System.RuntimeTypeHandle",
                            Reference = new ManagedInstructionReference
                            {
                                AssemblyName = assemblyName,
                                SubjectKind = "type",
                                SubjectId = tokenHolderTypeSubjectId,
                            },
                        },
                        new TypedIlInstructionArtifact
                        {
                            Op = "ldtoken",
                            Operand = fieldSubjectId,
                            ResultType = "System.RuntimeFieldHandle",
                            Reference = new ManagedInstructionReference
                            {
                                AssemblyName = assemblyName,
                                SubjectKind = "field",
                                SubjectId = fieldSubjectId,
                            },
                        },
                        new TypedIlInstructionArtifact
                        {
                            Op = "ldtoken",
                            Operand = helperMethodSubjectId,
                            ResultType = "System.RuntimeMethodHandle",
                            Reference = new ManagedInstructionReference
                            {
                                AssemblyName = assemblyName,
                                SubjectKind = "method",
                                SubjectId = helperMethodSubjectId,
                            },
                        },
                        new TypedIlInstructionArtifact
                        {
                            Op = "ret",
                        },
                    ],
                },
            ],
        },
    ],
};

var linkedWorld = new LinkedWorldModel
{
    InputAssemblyPath = "synthetic.dll",
    Assembly = assembly,
    Assemblies = [assembly],
    EntryPointSubjectId = mainMethodSubjectId,
    Types = [programType, tokenHolderType],
    Fields = [valueField],
    Properties = [],
    Methods = [helperMethod, mainMethod],
    Dependencies = [],
    CanonicalSubjects = new CanonicalSubjectsModel
    {
        Subjects = [],
    },
    SemanticShapes = new SemanticShapesModel
    {
        Types = [],
        Fields = [],
        Properties = [],
        Methods = [],
    },
    CapabilityBundles = new CapabilityBundlesModel
    {
        World = new WorldCapabilityBundleModel
        {
            Capabilities = [],
        },
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

var codeRegistration = new CodeRegistrationArtifact
{
    Modules =
    [
        new CodeRegistrationModule
        {
            ModuleName = assemblyName,
            Registrations =
            [
                new CodeRegistrationEntry
                {
                    RegistrationKind = "methodPointer",
                    Slot = 0,
                    Symbol = "chaos_helper",
                    SubjectId = helperMethodSubjectId,
                },
                new CodeRegistrationEntry
                {
                    RegistrationKind = "methodPointer",
                    Slot = 1,
                    Symbol = "chaos_main",
                    SubjectId = mainMethodSubjectId,
                },
            ],
        },
    ],
};

var aotCoreIr = new AotCoreIrLowering().Create(linkedWorld, typedIl, codeRegistration);
Console.WriteLine(JsonSerializer.Serialize(
    aotCoreIr,
    new JsonSerializerOptions
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    }));
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_synthetic_output_generated(self) -> None:
        if self.__class__.synthetic_output_generated:
            return

        project_root = self._write_synthetic_harness_project()
        project_path = project_root / "SyntheticLdtokenHarness.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / "SyntheticLdtokenHarness.dll"

        run_checked(["dotnet", "build", str(project_path), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing synthetic harness dll: {harness_dll_path}")
        completed = run_checked(["dotnet", str(harness_dll_path)], cwd=REPO_ROOT)
        self.synthetic_output_path.write_text(completed.stdout, encoding="utf-8")

        self.__class__.synthetic_output_generated = True
