from __future__ import annotations

import subprocess
import unittest
import uuid
import xml.etree.ElementTree as ET
from pathlib import Path

from tests.support import read_loader_stage_source


REPO_ROOT = Path(__file__).resolve().parents[3]
CORE_SOLUTION_PATH = REPO_ROOT / "solutions" / "core" / "windows" / "chaos-il2cpp-core.sln"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase7-interpreter-mixed-execution"

INTERPRETER_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Interpreter"
INTERPRETER_PROJECT_PATH = INTERPRETER_ROOT / "Chaos.IL2CPP.Interpreter.csproj"
INTERPRETER_IR_PATH = INTERPRETER_ROOT / "InterpreterIR.cs"
IL_TO_IR_LOWERING_PATH = INTERPRETER_ROOT / "ILToIRLowering.cs"
MANAGED_INTERPRETER_EXECUTOR_PATH = INTERPRETER_ROOT / "ManagedInterpreterExecutor.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
HOT_UPDATE_RUNTIME_MANAGER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "RuntimeManager.cs"
HOT_UPDATE_METHOD_REGISTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "HotUpdateMethodRegistry.cs"
INTERPRETER_ARITHMETIC_PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "MixedExecutionFeaturePack"
    / "source"
    / "EngineeringScenarios"
    / "MixedBridgeSolution"
    / "InterpreterArithmeticProof"
    / "InterpreterArithmeticProof.csproj"
)
MIXED_BRIDGE_SOLUTION_PATH = (
    REPO_ROOT
    / "subjects"
    / "MixedExecutionFeaturePack"
    / "source"
    / "EngineeringScenarios"
    / "MixedBridgeSolution"
    / "MixedBridgeSolution.sln"
)
INTERPRETER_LOWERING_ROOT = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack"
INTERPRETER_LOWERING_MANIFEST_PATH = INTERPRETER_LOWERING_ROOT / "subject.manifest.json"
INTERPRETER_LOWERING_PROJECT_PATH = INTERPRETER_LOWERING_ROOT / "source" / "MixedExecutionFeaturePack.csproj"
INTERPRETER_LOWERING_PROGRAM_PATH = INTERPRETER_LOWERING_ROOT / "source" / "Proofs" / "InterpreterLoweringProofEntry.cs"
MIXED_EXECUTION_PROOF_ROOT = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack"
MIXED_EXECUTION_PROOF_MANIFEST_PATH = MIXED_EXECUTION_PROOF_ROOT / "subject.manifest.json"
MIXED_EXECUTION_PROOF_PROJECT_PATH = MIXED_EXECUTION_PROOF_ROOT / "source" / "MixedExecutionFeaturePack.csproj"
MIXED_EXECUTION_PROOF_PROGRAM_PATH = MIXED_EXECUTION_PROOF_ROOT / "source" / "Proofs" / "MixedExecutionProofEntry.cs"
INTERPRETER_ARITHMETIC_PROOF_PATH = (
    MIXED_EXECUTION_PROOF_ROOT / "source" / "Proofs" / "InterpreterArithmeticProofEntry.cs"
)
MIXED_GENERIC_FLOW_PROOF_PATH = (
    MIXED_EXECUTION_PROOF_ROOT / "source" / "Proofs" / "MixedGenericFlowProofEntry.cs"
)
MIXED_EXCEPTION_FLOW_PROOF_PATH = (
    MIXED_EXECUTION_PROOF_ROOT / "source" / "Proofs" / "MixedExceptionFlowProofEntry.cs"
)
MIXED_DELEGATE_FLOW_PROOF_PATH = (
    MIXED_EXECUTION_PROOF_ROOT / "source" / "Proofs" / "MixedDelegateFlowProofEntry.cs"
)
MIXED_EXECUTION_HOST_PROGRAM_PATH = MIXED_EXECUTION_PROOF_ROOT / "source" / "Host" / "Program.cs"
NATIVE_INTERPRETER_ROOT = REPO_ROOT / "src" / "native" / "interpreter"
NATIVE_INTERPRETER_CMAKE_PATH = NATIVE_INTERPRETER_ROOT / "CMakeLists.txt"
NATIVE_INTERPRETER_HEADER_PATH = NATIVE_INTERPRETER_ROOT / "interpreter_vm.h"
NATIVE_INTERPRETER_SOURCE_PATH = NATIVE_INTERPRETER_ROOT / "interpreter_vm.cpp"
NATIVE_INTERPRETER_SMOKE_ROOT = REPO_ROOT / "tests" / "contracts" / "native" / "interpreter"
NATIVE_INTERPRETER_SMOKE_CMAKE_PATH = NATIVE_INTERPRETER_SMOKE_ROOT / "CMakeLists.txt"
NATIVE_INTERPRETER_SMOKE_SOURCE_PATH = NATIVE_INTERPRETER_SMOKE_ROOT / "interpreter_smoke.cpp"


def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


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


def make_unique_build_root(prefix: str) -> Path:
    build_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
    build_root.mkdir(parents=True, exist_ok=True)
    return build_root

class Phase7InterpreterMixedExecutionTestSupport(unittest.TestCase):
    pass
