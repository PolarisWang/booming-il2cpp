from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOLUTION_PATH = SUBJECT_ROOT / "source" / "SolutionCorePack.sln"
PRIMARY_PROJECT_PATH = SUBJECT_ROOT / "source" / "Host" / "SolutionCorePack.csproj"
ARCHETYPE_ROOT = SUBJECT_ROOT / "source" / "EngineeringScenarios"
REFERENCE_BUNDLE_ROOT = REPO_ROOT / "assets" / "reference-bundles" / "dotnet-foundation"
CONTROLLED_DLL_ROOT = REPO_ROOT / "src" / "dll" / "dotnet-foundation"
LAUNCHER_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Host" / "Program.cs"
CORELIB_NATIVE_PROOF_PROGRAM_PATH = (
    SUBJECT_ROOT
    / "source"
    / "EngineeringScenarios"
    / "CoreLibReferenceSolution"
    / "NativeProofApp"
    / "Program.cs"
)

class SolutionCorePackSubjectTestSupport(unittest.TestCase):
    pass
