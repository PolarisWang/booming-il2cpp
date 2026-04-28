from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any


CLASS_RE = re.compile(r"\bclass\s+(?P<name>[A-Za-z0-9_]+)")
METHOD_CASE_RE = re.compile(
    r'\[MethodSubjectId\("(?P<subject>[^"]+)"\)\]\s*'
    r'(?:\[[^\]]+\]\s*)*'
    r'public\s+(?:static\s+)?(?:async\s+)?[A-Za-z0-9_<>,\.\?]+\s+(?P<member>[A-Za-z0-9_]+)\s*\(',
    re.MULTILINE,
)
BENCHMARK_CASE_RE = re.compile(
    r'\[BenchmarkSubjectId\("(?P<subject>[^"]+)"\)\]\s*'
    r'(?:\[[^\]]+\]\s*)*'
    r'public\s+static\s+[A-Za-z0-9_<>,\.\?]+\s+(?P<member>[A-Za-z0-9_]+)\s*\(',
    re.MULTILINE,
)
HOTUPDATE_CASE_RE = re.compile(
    r'\[HotUpdateSubjectId\("(?P<subject>[^"]+)"\)\]\s*'
    r'(?:\[[^\]]+\]\s*)*'
    r'public\s+static\s+[A-Za-z0-9_<>,\.\?]+\s+(?P<member>[A-Za-z0-9_]+)\s*\(',
    re.MULTILINE,
)
VERIFY_ROUTE_RE = re.compile(r"\[VerificationRoute\(VerificationRoute\.(?P<route>[A-Za-z0-9_]+)\)\]")
BENCH_ROUTE_RE = re.compile(r"\[BenchmarkRoute\(BenchmarkRoute\.(?P<route>[A-Za-z0-9_]+)\)\]")
BENCH_PROFILE_RE = re.compile(r"\[BenchmarkProfile\(BenchmarkProfile\.(?P<profile>[A-Za-z0-9_]+)\)\]")
HOTUPDATE_DIR_RE = re.compile(r"\[HotUpdateDirection\(HotUpdateDirection\.(?P<direction>[A-Za-z0-9_]+)\)\]")


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def _scan_source(path: Path) -> dict[str, Any]:
    text = path.read_text(encoding="utf-8")
    class_name_match = CLASS_RE.search(text)
    return {
        "className": class_name_match.group("name") if class_name_match is not None else path.stem,
        "methodCases": [
            {"methodSubjectId": match.group("subject"), "memberName": match.group("member")}
            for match in METHOD_CASE_RE.finditer(text)
        ],
        "benchmarkCases": [
            {"methodSubjectId": match.group("subject"), "memberName": match.group("member")}
            for match in BENCHMARK_CASE_RE.finditer(text)
        ],
        "hotupdateCases": [
            {"methodSubjectId": match.group("subject"), "memberName": match.group("member")}
            for match in HOTUPDATE_CASE_RE.finditer(text)
        ],
        "verificationRoutes": [m.group("route") for m in VERIFY_ROUTE_RE.finditer(text)],
        "benchmarkRoutes": [m.group("route") for m in BENCH_ROUTE_RE.finditer(text)],
        "benchmarkProfiles": [m.group("profile") for m in BENCH_PROFILE_RE.finditer(text)],
        "hotupdateDirections": [m.group("direction") for m in HOTUPDATE_DIR_RE.finditer(text)],
    }


def scan_case_indexes(repo_root: Path, *, assembly_name: str, family_id: str) -> dict[str, Any]:
    family_slug = "-".join(family_id.split("/")[2:])
    family_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug

    cases_test: list[dict[str, Any]] = []
    for path in sorted((family_root / "test").glob("*.cs")):
        scan = _scan_source(path)
        for item in scan["methodCases"]:
            cases_test.append(
                {
                    "methodSubjectId": item["methodSubjectId"],
                    "familyId": family_id,
                    "caseKind": "functional-test",
                    "projectPath": str((family_root / "test").relative_to(repo_root)).replace("\\", "/"),
                    "sourceFile": str(path.relative_to(repo_root)).replace("\\", "/"),
                    "className": scan["className"],
                    "memberName": item["memberName"],
                    "routeCode": (scan["verificationRoutes"][0] if scan["verificationRoutes"] else ""),
                    "profileCode": "",
                    "annotationDigest": "generated-source",
                }
            )

    cases_benchmark: list[dict[str, Any]] = []
    for path in sorted((family_root / "benchmark").glob("*.cs")):
        scan = _scan_source(path)
        for item in scan["benchmarkCases"]:
            cases_benchmark.append(
                {
                    "methodSubjectId": item["methodSubjectId"],
                    "familyId": family_id,
                    "caseKind": "benchmark",
                    "projectPath": str((family_root / "benchmark").relative_to(repo_root)).replace("\\", "/"),
                    "sourceFile": str(path.relative_to(repo_root)).replace("\\", "/"),
                    "className": scan["className"],
                    "memberName": item["memberName"],
                    "routeCode": (scan["benchmarkRoutes"][0] if scan["benchmarkRoutes"] else ""),
                    "profileCode": (scan["benchmarkProfiles"][0] if scan["benchmarkProfiles"] else ""),
                    "annotationDigest": "generated-source",
                }
            )

    cases_hotupdate: list[dict[str, Any]] = []
    for path in sorted((family_root / "host").glob("*.cs")):
        scan = _scan_source(path)
        for item in scan["hotupdateCases"]:
            cases_hotupdate.append(
                {
                    "methodSubjectId": item["methodSubjectId"],
                    "familyId": family_id,
                    "caseKind": "hotupdate",
                    "projectPath": str((family_root / "host").relative_to(repo_root)).replace("\\", "/"),
                    "sourceFile": str(path.relative_to(repo_root)).replace("\\", "/"),
                    "className": scan["className"],
                    "memberName": item["memberName"],
                    "direction": (scan["hotupdateDirections"][0] if scan["hotupdateDirections"] else ""),
                    "annotationDigest": "generated-source",
                }
            )

    test_index_path = family_root / "method-test-case-index.json"
    benchmark_index_path = family_root / "method-benchmark-case-index.json"
    hotupdate_index_path = family_root / "method-hotupdate-case-index.json"

    _write_json(test_index_path, {"schemaVersion": 1, "cases": cases_test})
    _write_json(benchmark_index_path, {"schemaVersion": 1, "cases": cases_benchmark})
    _write_json(hotupdate_index_path, {"schemaVersion": 1, "cases": cases_hotupdate})

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": [
            test_index_path.relative_to(repo_root).as_posix(),
            benchmark_index_path.relative_to(repo_root).as_posix(),
            hotupdate_index_path.relative_to(repo_root).as_posix(),
        ],
    }
