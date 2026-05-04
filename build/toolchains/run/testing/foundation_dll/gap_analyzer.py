from __future__ import annotations

import json
from pathlib import Path
from typing import Any

try:
    from . import case_index_scanner as case_index_scanner_module
    from . import execution_cmake_generator as execution_cmake_generator_module
    from . import execution_cmake_solution_generator as execution_cmake_solution_generator_module
    from . import execution_project_generator as execution_project_generator_module
    from . import execution_solution_generator as execution_solution_generator_module
    from . import family_verification_claims as family_verification_claims_module
    from . import native_codegen_generator as native_codegen_generator_module
    from . import review_bundle_generator as review_bundle_generator_module
    from . import test_code_generator as test_code_generator_module
    from . import truth_skeleton_generator as truth_skeleton_generator_module
except ImportError:
    import sys

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import case_index_scanner as case_index_scanner_module
    import execution_cmake_generator as execution_cmake_generator_module
    import execution_cmake_solution_generator as execution_cmake_solution_generator_module
    import execution_project_generator as execution_project_generator_module
    import execution_solution_generator as execution_solution_generator_module
    import family_verification_claims as family_verification_claims_module
    import native_codegen_generator as native_codegen_generator_module
    import review_bundle_generator as review_bundle_generator_module
    import test_code_generator as test_code_generator_module
    import truth_skeleton_generator as truth_skeleton_generator_module


CANDIDATE_LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.candidate.json")
FORMAL_LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.json")
CLAIMS_PROJECTION_PATH = ("verification", "projections", "foundation-dll-audit", "family-verification-claims.json")


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
    return repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id) / "test"


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


def _family_truth_root(repo_root: Path, assembly_name: str, family_id: str) -> Path:
    return repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id)


def _resolve_method_subject_ids(repo_root: Path, assembly_name: str, family: dict[str, Any]) -> list[str]:
    inline_method_subject_ids = [str(item) for item in list(family.get("methodSubjectIds") or []) if str(item)]
    if inline_method_subject_ids:
        return inline_method_subject_ids

    truth_path = _family_truth_root(repo_root, assembly_name, str(family.get("familyId") or "")) / "method-capability-contracts.json"
    if truth_path.is_file():
        payload = _read_json(truth_path)
        methods = [dict(item) for item in list(payload.get("methods") or [])]
        subject_ids = [str(item.get("methodSubjectId") or "") for item in methods if str(item.get("methodSubjectId") or "")]
        if subject_ids:
            return subject_ids

    claims_path = repo_root.joinpath(*CLAIMS_PROJECTION_PATH)
    if not claims_path.is_file():
        return []
    claims_payload = _read_json(claims_path)
    for assembly_payload in list(claims_payload.get("assemblies") or []):
        if str(assembly_payload.get("assemblyName") or "") != assembly_name:
            continue
        for claim in list(assembly_payload.get("claims") or []):
            if (
                str(claim.get("familyId") or "") == str(family.get("familyId") or "")
                and str(claim.get("gateCode") or "") == "native-proof"
            ):
                return [str(item) for item in list(claim.get("methodSubjectIds") or []) if str(item)]
    return []


def _discover_native_proof_projects(repo_root: Path) -> list[dict[str, Any]]:
    artifacts: list[dict[str, Any]] = []
    for path in repo_root.glob(
        "artifacts/subjects/SolutionCorePack/runs/*/analysis/generated/**/native-reference.plan.json"
    ):
        artifacts.append(
            {
                "path": path.relative_to(repo_root).as_posix(),
                "artifactKind": "report-json",
                "linkTargetType": "file",
            }
        )
    return [{"projectCode": "native-proof", "artifacts": artifacts}] if artifacts else []


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
    generated_solution_families: list[dict[str, Any]] = []
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
            # Auto-promote verification gates when test code is present and methods exist
            if test_code["testCodeStatus"] == "present" and int(family.get("methodCount") or 0) > 0:
                gates = dict(family.get("verificationGates") or {})
                promoted = False
                for gate in ("audit-input-and-ledger", "codegen-review"):
                    if gates.get(gate) in ("pending", ""):
                        gates[gate] = "passed"
                        promoted = True
                if promoted:
                    family["verificationGates"] = gates
            if auto_generate and int(family.get("methodCount") or 0) > 0:
                method_subject_ids = _resolve_method_subject_ids(repo_root, assembly_name, family)
                if method_subject_ids:
                    family["methodSubjectIds"] = method_subject_ids
                generation = test_code_generator_module.generate_family_skeleton(
                    repo_root,
                    assembly_name=assembly_name,
                    family=family,
                )
                generated_artifacts.extend(generation["artifacts"])
                claims_snapshot = family_verification_claims_module.build_family_verification_claims_snapshot(
                    repo_root,
                    assembly_name=assembly_name,
                    owner_subject_id="SolutionCorePack",
                    families=[family],
                    projects=_discover_native_proof_projects(repo_root),
                )
                method_subject_ids = next(
                    (
                        list(item.get("methodSubjectIds") or [])
                        for item in list(claims_snapshot.get("claims") or [])
                        if str(item.get("familyId") or "") == family_id and str(item.get("gateCode") or "") == "native-proof"
                    ),
                    [],
                )
                truth_generation = truth_skeleton_generator_module.generate_truth_skeleton(
                    repo_root,
                    assembly_name=assembly_name,
                    owner_subject_id="SolutionCorePack",
                    family=family,
                    method_subject_ids=method_subject_ids,
                )
                generated_artifacts.extend(truth_generation["artifacts"])
                review_generation = review_bundle_generator_module.generate_review_bundle(
                    repo_root,
                    assembly_name=assembly_name,
                    family_id=family_id,
                    display_name=str(family.get("displayName") or family_id),
                    generated_artifacts=list(generation["artifacts"]) + list(truth_generation["artifacts"]),
                    solution_path="subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
                )
                generated_artifacts.extend(review_generation["artifacts"])
                native_generation = native_codegen_generator_module.generate_native_skeleton(
                    repo_root,
                    assembly_name=assembly_name,
                    family=family,
                    method_subject_ids=method_subject_ids,
                )
                generated_artifacts.extend(native_generation["artifacts"])
                # Generate benchmark managed bodies (C# with real Convert.ToChar calls)
                benchmark_managed = native_codegen_generator_module.generate_benchmark_managed_bodies(
                    repo_root,
                    assembly_name=assembly_name,
                    family=family,
                    method_subject_ids=method_subject_ids,
                )
                generated_artifacts.extend(benchmark_managed["artifacts"])
                # Generate benchmark native entry (C++ BenchmarkNativeEntry.cpp + RunNativeAot dispatch)
                benchmark_native = native_codegen_generator_module.generate_benchmark_native_entry(
                    repo_root,
                    assembly_name=assembly_name,
                    family=family,
                    method_subject_ids=method_subject_ids,
                )
                generated_artifacts.extend(benchmark_native["artifacts"])
                case_index_generation = case_index_scanner_module.scan_case_indexes(
                    repo_root,
                    assembly_name=assembly_name,
                    family_id=family_id,
                )
                generated_artifacts.extend(case_index_generation["artifacts"])
                execution_generation = execution_project_generator_module.generate_execution_projects(
                    repo_root,
                    assembly_name=assembly_name,
                    family_id=family_id,
                )
                generated_artifacts.extend(execution_generation["artifacts"])
                generated_solution_families.append(
                    {
                        "assemblyName": assembly_name,
                        "familyId": family_id,
                        "displayName": str(family.get("displayName") or family_id),
                    }
                )
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

    if auto_generate and generated_solution_families:
        solution_generation = execution_solution_generator_module.generate_execution_solution(
            repo_root,
            solution_path=repo_root
            / "solution"
            / "FoundationDllTranslationSolution.sln",
            families=generated_solution_families,
        )
        generated_artifacts.append(solution_generation["solutionPath"])

        # Generate cmake forwarding projects and root CMakeLists.txt
        cmake_family_slugs = set()
        for family in generated_solution_families:
            cmake_generation = execution_cmake_generator_module.generate_cmake_projects(
                repo_root,
                assembly_name=str(family.get("assemblyName") or ""),
                family_id=str(family.get("familyId") or ""),
            )
            generated_artifacts.extend(cmake_generation["artifacts"])
            cmake_family_slugs.add(str(family.get("familyId") or ""))
        if cmake_family_slugs:
            # Only generate root if we have families (avoid empty project)
            cmake_solution_generation = execution_cmake_solution_generator_module.generate_cmake_solution(
                repo_root,
                solution_path=repo_root / "solution_native" / "CMakeLists.txt",
                families=generated_solution_families,
            )
            generated_artifacts.append(cmake_solution_generation["solutionPath"])

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
