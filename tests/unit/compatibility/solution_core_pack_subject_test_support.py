from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
OWNER_ROOT = REPO_ROOT / "verification" / "catalog" / "owners" / "SolutionCorePack"
SCENARIO_ROOT = REPO_ROOT / "verification" / "catalog" / "scenarios" / "SolutionCorePack"
SUBJECT_ROOT = OWNER_ROOT
MANIFEST_PATH = OWNER_ROOT / "owner.manifest.json"
SOLUTION_PATH = OWNER_ROOT / "support" / "host" / "SolutionCorePack.sln"
PRIMARY_PROJECT_PATH = OWNER_ROOT / "support" / "host" / "SolutionCorePack.csproj"
ARCHETYPE_ROOT = SCENARIO_ROOT
REFERENCE_BUNDLE_ROOT = REPO_ROOT / "assets" / "reference-bundles" / "dotnet-foundation"
CONTROLLED_DLL_ROOT = REPO_ROOT / "src" / "dll" / "dotnet-foundation"
LAUNCHER_PROGRAM_PATH = OWNER_ROOT / "support" / "host" / "Program.cs"
CORELIB_NATIVE_PROOF_PROGRAM_PATH = (
    SCENARIO_ROOT
    / "CoreLibReferenceSolution"
    / "NativeProofApp"
    / "Program.cs"
)

class SolutionCorePackSubjectTestSupport(unittest.TestCase):
    pass
