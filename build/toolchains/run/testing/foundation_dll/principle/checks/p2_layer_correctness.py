"""P2 (monitoring): Verify changed files touch expected architecture layers.

Uses git diff to detect whether source-level changes respect
the expected layer distribution (Emitter vs Planner vs RuntimeSupport vs RuntimeCore).
Only considers files under src/managed/ and src/native/; generated verification
output artifacts are excluded.
"""

from __future__ import annotations

import re
import subprocess
from pathlib import Path

from ..base import CheckMeta, CheckResult, FamilyContext, PrincipleCheck, _REPO_ROOT

LAYER_RULES: dict[str, re.Pattern] = {
    "Emitter": re.compile(r"src/managed/.*CodeGen/Emission/"),
    "Planner": re.compile(r"src/managed/.*CodeGen/Planning/"),
    "RuntimeSupport": re.compile(r"src/managed/.*CodeGen/RuntimeSupport/"),
    "Templates": re.compile(r"src/managed/.*CodeGen/Templates/"),
    "Driver": re.compile(r"src/managed/.*Chaos\.IL2CPP\.Driver/"),
    "CodeGenRoot": re.compile(r"src/managed/.*Chaos\.IL2CPP\.CodeGen/"),
    "ManagedContracts": re.compile(r"src/managed/.*Chaos\.IL2CPP\.Contracts/"),
    "RuntimeCore": re.compile(r"src/native/runtime-core/"),
    "Interpreter": re.compile(r"src/native/interpreter/"),
    "Bootstrap": re.compile(r"src/native/bootstrap/"),
    "Common": re.compile(r"src/native/common/"),
    "Codegen": re.compile(r"src/native/codegen/"),
    "HotUpdate": re.compile(r"src/native/hot-update/"),
    "Contracts": re.compile(r"contracts/"),
    "BuildConfig": re.compile(r"CMakeLists\.txt$"),
    "Tests": re.compile(r"tests/"),
    "TestInfra": re.compile(r"build/toolchains/run/testing/"),
}

KNOWN_GENERATED_PATTERNS = [
    re.compile(r"verification/foundation-dll/"),
    re.compile(r"\.generated\."),
    re.compile(r"runtime_helper_shapes\."),
    re.compile(r"closure\.manifest\."),
    re.compile(r"codegen-metrics\."),
    re.compile(r"\.patchdata$"),
    re.compile(r"batch_verify_results\.json$"),
    re.compile(r"session_outcomes\.jsonl$"),
    re.compile(r"skills/"),
    re.compile(r"^docs/"),
]


def _is_generated_artifact(file_path: str) -> bool:
    """Check if a file path is a generated artifact (not source code)."""
    return any(p.search(file_path) for p in KNOWN_GENERATED_PATTERNS)


def _get_repo_changed_files() -> list[str]:
    """Get all changed source files from git diff HEAD."""
    try:
        result = subprocess.run(
            ["git", "diff", "--name-only", "HEAD"],
            capture_output=True, encoding="utf-8", cwd=_REPO_ROOT, timeout=30,
        )
        if result.stdout is None:
            return []
        return [f.strip() for f in result.stdout.strip().split("\n") if f.strip()]
    except (subprocess.SubprocessError, OSError):
        return []


def _assign_layer(file_path: str) -> str | None:
    """Assign an architecture layer to a source file path based on LAYER_RULES."""
    for layer, pattern in LAYER_RULES.items():
        if pattern.search(file_path):
            return layer
    return None


class P2LayerCorrectnessCheck(PrincipleCheck):
    meta = CheckMeta(
        check_id="p2_layer_correctness",
        principle="P2",
        severity="monitoring",
        scope="family",
        description="Changed files' directory distribution matches expected architecture layers",
    )

    def run(self, ctx: FamilyContext) -> CheckResult:
        # Get repo-wide changed files and filter to source files only
        all_changed = _get_repo_changed_files()
        source_files = [f for f in all_changed if not _is_generated_artifact(f)]

        # Ignore deleted files — they don't affect architecture layer distribution.
        source_files = [f for f in source_files if (_REPO_ROOT / f).exists()]

        if not source_files:
            return CheckResult(
                check_id="p2_layer_correctness", status="NOT_APPLICABLE",
                summary="No changed source files detected",
            )

        violations = []
        layer_counts: dict[str, int] = {}

        for f in source_files:
            layer = _assign_layer(f)
            if layer is None:
                violations.append({
                    "file": str(f),
                    "reason": "outside expected architecture layers",
                })
            else:
                layer_counts[layer] = layer_counts.get(layer, 0) + 1

        if violations:
            return CheckResult(
                check_id="p2_layer_correctness", status="CONCERN",
                summary=f"{len(violations)} of {len(source_files)} changed source files are "
                        f"outside expected architecture layers",
                details=violations,
                evidence={
                    "changed_source_count": len(source_files),
                    "layer_distribution": layer_counts,
                    "violations": violations,
                },
            )

        return CheckResult(
            check_id="p2_layer_correctness", status="ALIGNED",
            summary=f"All {len(source_files)} changed source files within expected architecture layers",
            evidence={
                "changed_source_count": len(source_files),
                "layer_distribution": layer_counts,
            },
        )
