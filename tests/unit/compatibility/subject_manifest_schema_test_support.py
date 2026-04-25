from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"
RETAINED_SOLUTION_SUBJECT_IDS = {
    "SolutionCorePack",
    "HotUpdateHostPack",
    "MixedExecutionFeaturePack",
}
SOLUTION_CORE_PACK_PIPELINE_IDS = {
    "managed-runtime-output",
    "managed-runtime-trace",
    "proof-runtime-output",
    "native-hotupdate-proof-output",
    "managed-benchmark",
    "native-benchmark",
}
SOLUTION_CORE_PACK_MATRIX_IDS = {
    "windows-archetype-simple-lib-managed-output",
    "windows-archetype-multi-project-managed-output",
    "windows-archetype-package-reference-managed-output",
    "windows-archetype-reference-assembly-managed-output",
    "windows-archetype-corelib-reference-managed-output",
    "windows-corelib-reference-native-proof",
    "windows-corelib-reference-hotupdate-proof",
    "windows-corelib-reference-native-hotupdate-proof",
    "windows-archetype-mixed-reference-closure-managed-output",
    "windows-foundation-dll-translation-native-proof",
    "windows-static-call-ctor-getter-native-proof",
    "windows-constructor-then-instance-call-native-proof",
    "windows-delegate-closed-target-relay-native-proof",
    "windows-exception-throw-catch-finally-native-proof",
    "windows-nested-exception-throw-catch-finally-native-proof",
    "windows-marshaling-utf8-export-native-proof",
    "windows-reflection-interop-closure-native-proof",
    "windows-pinvoke-direct-call-native-proof",
    "windows-array-boxing-reference-array-native-proof",
    "windows-interface-dispatch-message-native-proof",
    "windows-threading-threadstatic-monitor-native-proof",
    "windows-native-check",
    "windows-managed-proof",
    "windows-managed-trace",
    "macos-managed-trace",
    "windows-managed-perf",
    "windows-native-perf",
}
SOLUTION_CORE_PACK_HOST_SOLUTION = "verification/catalog/owners/SolutionCorePack/support/host/SolutionCorePack.sln"
SOLUTION_CORE_PACK_HOST_PROJECT = "verification/catalog/owners/SolutionCorePack/support/host/SolutionCorePack.csproj"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def assert_matrix_source_entry(
    test_case: unittest.TestCase,
    subjects_module,
    manifest: dict[str, object],
    matrix_id: str,
    expected_entry: str,
) -> None:
    matrix = subjects_module.find_matrix(manifest, matrix_id)
    test_case.assertEqual(expected_entry, str(dict(matrix.get("source") or {})["entry"]))


def owner_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return repo_root / "verification" / "catalog" / "owners" / subject_id / "owner.manifest.json"


def write_owner_manifest(repo_root: Path, subject_id: str, payload: dict) -> Path:
    manifest_path = owner_manifest_path(repo_root, subject_id)
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    (manifest_path.parent / "owner.features.json").write_text(
        json.dumps(
            {
                "subjectId": subject_id,
                "features": [],
            },
            ensure_ascii=False,
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    return manifest_path

class SubjectManifestSchemaTestSupport(unittest.TestCase):
    pass
