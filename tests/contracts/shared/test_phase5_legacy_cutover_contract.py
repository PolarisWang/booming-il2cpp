from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CANONICAL_SUBJECTS = (
    "SolutionCorePack",
    "MixedExecutionFeaturePack",
    "HotUpdateHostPack",
)
BANNED_SUBJECT_SEGMENTS = {
    "validation",
    "Archetypes",
    "FeatureSlices",
    "PatchModules",
    "ManagedBridge",
    "Lowering",
    "Launcher",
}
BANNED_PRODUCT_SYMBOLS = (
    "ChaosSourceEntryArguments",
    "DeclaredProofEntriesBySourceEntry",
    "MainlineFeaturePack",
    "PerformanceFeaturePack",
)
BANNED_CORRECTNESS_SYMBOLS = (
    "Console.WriteLine",
    "ChaosEvidenceKind.Stdout",
)
FORMAL_VERIFICATION_ROOTS = {
    "SolutionCorePack": (
        REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Proofs",
        REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Benchmarks",
        REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Host",
    ),
    "MixedExecutionFeaturePack": (
        REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "Proofs",
        REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "Benchmarks",
        REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "Host",
    ),
    "HotUpdateHostPack": (
        REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "Host" / "Proofs",
        REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "Host" / "Benchmarks",
        REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "Host",
    ),
}
PRODUCT_SOURCE_ROOTS = (
    REPO_ROOT / "build" / "toolchains" / "run",
    REPO_ROOT / "src" / "reference",
    REPO_ROOT / "subjects" / "SolutionCorePack",
    REPO_ROOT / "subjects" / "MixedExecutionFeaturePack",
    REPO_ROOT / "subjects" / "HotUpdateHostPack",
)
IGNORED_SOURCE_SEGMENTS = {
    ".vs",
    "bin",
    "obj",
    "artifacts",
    "__pycache__",
}


def _iter_source_files(root: Path) -> list[Path]:
    files: list[Path] = []
    for pattern in ("*.py", "*.cs", "*.json", "*.csproj", "*.sln", "*.tmpl"):
        files.extend(root.rglob(pattern))
    return sorted(
        path
        for path in files
        if path.is_file() and not (set(path.relative_to(root).parts) & IGNORED_SOURCE_SEGMENTS)
    )


class Phase5LegacyCutoverContractTests(unittest.TestCase):
    def test_canonical_subject_trees_do_not_reintroduce_legacy_directory_segments(self) -> None:
        for subject_id in CANONICAL_SUBJECTS:
            subject_root = REPO_ROOT / "subjects" / subject_id
            self.assertTrue(subject_root.is_dir(), msg=f"missing canonical subject root: {subject_root}")

            for path in subject_root.rglob("*"):
                relative_parts = set(path.relative_to(subject_root).parts)
                intersection = relative_parts & BANNED_SUBJECT_SEGMENTS
                self.assertEqual(
                    set(),
                    intersection,
                    msg=f"{subject_id} reintroduced legacy path segments in {path.relative_to(REPO_ROOT).as_posix()}",
                )

    def test_product_pipeline_sources_do_not_reference_legacy_subject_protocol_symbols(self) -> None:
        hits: list[str] = []

        for root in PRODUCT_SOURCE_ROOTS:
            self.assertTrue(root.exists(), msg=f"missing product source root: {root}")
            for path in _iter_source_files(root):
                source = path.read_text(encoding="utf-8")
                for symbol in BANNED_PRODUCT_SYMBOLS:
                    if symbol in source:
                        hits.append(f"{path.relative_to(REPO_ROOT).as_posix()} :: {symbol}")

        self.assertEqual([], hits)

    def test_formal_subject_verification_assets_do_not_use_console_correctness(self) -> None:
        hits: list[str] = []

        for subject_id, roots in FORMAL_VERIFICATION_ROOTS.items():
            for root in roots:
                self.assertTrue(root.exists(), msg=f"missing formal verification root for {subject_id}: {root}")
                for path in root.rglob("*.cs"):
                    source = path.read_text(encoding="utf-8")
                    for symbol in BANNED_CORRECTNESS_SYMBOLS:
                        if symbol in source:
                            hits.append(f"{path.relative_to(REPO_ROOT).as_posix()} :: {symbol}")

        self.assertEqual([], hits)


if __name__ == "__main__":
    unittest.main()
