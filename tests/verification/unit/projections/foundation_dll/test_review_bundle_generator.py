from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
GENERATOR_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "review_bundle_generator.py"
)


def test_generate_review_bundle_creates_minimal_reviewable_bundle() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_review_bundle_generator")
    repo_root = make_temp_repo_root("verification", "foundation-dll-review-bundle")

    try:
        result = generator_module.generate_review_bundle(
            repo_root,
            assembly_name="System.Private.CoreLib",
            family_id="family/System.Private.CoreLib/convert/char",
            display_name="Convert.Char Conversions",
            generated_artifacts=[
                "verification/foundation-dll/System.Private.CoreLib/convert-char/method-test-case-index.json",
                "verification/foundation-dll/System.Private.CoreLib/convert-char/method-benchmark-case-index.json",
            ],
            solution_path="subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
        )

        review_root = (
            repo_root
            / "verification"
            / "foundation-dll"
            / "System.Private.CoreLib"
            / "convert-char"
            / "review"
        )
        bundle = json.loads((review_root / "bundle.json").read_text(encoding="utf-8"))

        assert review_root.is_dir()
        assert bundle["schemaVersion"] == 1
        assert bundle["assemblyName"] == "System.Private.CoreLib"
        assert bundle["familyId"] == "family/System.Private.CoreLib/convert/char"
        assert bundle["displayName"] == "Convert.Char Conversions"
        assert bundle["generatedArtifacts"]
        assert bundle["solutionPath"].endswith("FoundationDllTranslationSolution.sln")
        assert "verification/foundation-dll/System.Private.CoreLib/convert-char/review/bundle.json" in result["artifacts"]
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
