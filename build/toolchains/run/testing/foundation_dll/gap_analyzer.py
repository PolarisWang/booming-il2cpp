from __future__ import annotations

import json
from pathlib import Path
from typing import Any

try:
    from . import test_code_generator as test_code_generator_module
except ImportError:
    import sys

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import test_code_generator as test_code_generator_module


CANDIDATE_LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.candidate.json")
FORMAL_LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.json")


def _read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) < 4:
        return str(family_id).replace("/", "-")
    return "-".join(parts[2:])


def _family_test_dir(repo_root: Path, assembly_name: str, family_id: str) -> Path:
    return repo_root / "tests" / "families" / assembly_name / _family_slug(family_id)


def _derive_test_code_status(repo_root: Path, assembly_name: str, family: dict[str, Any]) -> dict[str, Any]:
    family_id = str(family.get("familyId") or "")
    method_count = int(family.get("methodCount") or 0)
    test_dir = _family_test_dir(repo_root, assembly_name, family_id)
    has_tests = test_dir.is_dir()
    existing = dict(family.get("testCode") or {})
    previous_status = str(existing.get("testCodeStatus") or "")

    if method_count <= 0:
        status = "no-coverage"
        action = "none"
    elif not has_tests:
        status = "needs-tests"
        action = "write-tests"
    elif previous_status in {"needs-tests", "coverage-widened"}:
        status = "coverage-widened"
        action = "update-tests"
    else:
        status = "present"
        action = "none"

    return {
        "testCodeStatus": status,
        "requestedMethodCount": method_count,
        "emittedMethodCount": method_count,
        "allMethodsCovered": has_tests and method_count > 0,
        "action": action,
        "testProjectPath": test_dir.relative_to(repo_root).as_posix() if has_tests else "",
    }


def _matches_scope(assembly_name: str, family_id: str, scope: str) -> bool:
    if scope in {"", "all"}:
        return True
    return scope == assembly_name or scope == family_id


def _existing_ledger_paths(repo_root: Path) -> list[Path]:
    paths = [
        repo_root.joinpath(*CANDIDATE_LEDGER_PATH),
        repo_root.joinpath(*FORMAL_LEDGER_PATH),
    ]
    return [path for path in paths if path.is_file()]


def _select_primary_ledger_path(repo_root: Path) -> Path:
    candidate = repo_root.joinpath(*CANDIDATE_LEDGER_PATH)
    if candidate.is_file():
        return candidate
    return repo_root.joinpath(*FORMAL_LEDGER_PATH)


def _analyze_payload(
    repo_root: Path,
    payload: dict[str, Any],
    *,
    scope: str,
    auto_generate: bool,
) -> tuple[dict[str, Any], list[str]]:
    results: list[dict[str, Any]] = []
    generated_artifacts: list[str] = []
    dll_count = 0
    status_counts = {
        "present": 0,
        "coverage-widened": 0,
        "needs-tests": 0,
        "no-coverage": 0,
    }

    for dll in list(payload.get("dlls") or []):
        assembly_name = str(dll.get("assemblyName") or "")
        matched_any = False
        for family in list(dll.get("families") or []):
            family_id = str(family.get("familyId") or "")
            if not _matches_scope(assembly_name, family_id, scope):
                continue
            matched_any = True
            test_code = _derive_test_code_status(repo_root, assembly_name, family)
            family["testCode"] = test_code
            if auto_generate and test_code["action"] in {"write-tests", "update-tests"}:
                generation = test_code_generator_module.generate_family_skeleton(
                    repo_root,
                    assembly_name=assembly_name,
                    family=family,
                )
                generated_artifacts.extend(generation["artifacts"])
                test_code = _derive_test_code_status(repo_root, assembly_name, family)
                family["testCode"] = test_code
            status_counts[test_code["testCodeStatus"]] += 1
            results.append(
                {
                    "assemblyName": assembly_name,
                    "familyId": family_id,
                    "displayName": str(family.get("displayName") or family_id),
                    "testCode": dict(test_code),
                }
            )
        if matched_any:
            dll_count += 1

    return {
        "dllCount": dll_count,
        "familyCount": len(results),
        "statusCounts": status_counts,
        "families": results,
    }, generated_artifacts


def analyze_gaps(
    repo_root: Path,
    *,
    scope: str = "all",
    update_ledger: bool = False,
    auto_generate: bool = False,
) -> dict[str, Any]:
    primary_ledger_path = _select_primary_ledger_path(repo_root)
    primary_payload = _read_json(primary_ledger_path)
    analysis, generated_artifacts = _analyze_payload(
        repo_root,
        primary_payload,
        scope=scope,
        auto_generate=auto_generate,
    )

    if update_ledger:
        for ledger_path in _existing_ledger_paths(repo_root):
            payload = _read_json(ledger_path)
            _analyze_payload(repo_root, payload, scope=scope, auto_generate=False)
            _write_json(ledger_path, payload)

    return {
        "scope": scope,
        "ledgerPath": primary_ledger_path.relative_to(repo_root).as_posix(),
        "dllCount": analysis["dllCount"],
        "familyCount": analysis["familyCount"],
        "statusCounts": analysis["statusCounts"],
        "families": analysis["families"],
        "generatedArtifacts": generated_artifacts,
    }
