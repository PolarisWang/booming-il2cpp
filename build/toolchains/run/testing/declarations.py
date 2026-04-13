from __future__ import annotations

from dataclasses import dataclass
from enum import Enum
from typing import Any, Iterable


class TestDeclarationMode(str, Enum):
    AUTO = "auto"
    NONE = "none"
    REQUIRE = "require"


class DeclaredTestKind(str, Enum):
    UNIT = "unit"
    BENCHMARK = "benchmark"


class SubjectDeclarationKind(str, Enum):
    RUNTIME_ONLY = "runtime-only"
    DECLARED_TEST = "declared-test"


class DeclarationWarningCode(str, Enum):
    FRAMEWORK_WITHOUT_DECLARATIONS = "framework-without-declarations"


@dataclass(frozen=True)
class DeclaredTestEntry:
    kind: DeclaredTestKind
    stable_id: str
    alias: str | None = None
    assembly_name: str = ""
    declaring_type: str = ""
    method_name: str = ""
    method_signature: str = ""
    category: int = 0
    metrics: int = 0
    requires: int = 0
    evidence: int = 0
    priority: int = 0
    warmup_count: int = 0
    iteration_count: int = 0
    invocation_count: int = 0


@dataclass(frozen=True)
class DeclarationScanSummary:
    mode: TestDeclarationMode
    framework_referenced: bool
    declared_unit_tests: list[DeclaredTestEntry]
    declared_benchmarks: list[DeclaredTestEntry]
    subject_kind: SubjectDeclarationKind
    warning_codes: list[DeclarationWarningCode]


def _normalize_declared_entries(
    values: Iterable[DeclaredTestEntry | dict[str, Any]] | None,
    *,
    kind: DeclaredTestKind,
) -> list[DeclaredTestEntry]:
    entries: list[DeclaredTestEntry] = []
    for value in list(values or []):
        if isinstance(value, DeclaredTestEntry):
            entries.append(value)
            continue

        stable_id = str(dict(value).get("stableId") or dict(value).get("stable_id") or "")
        if not stable_id:
            raise ValueError("declared entry requires stableId")

        payload = dict(value)
        alias_text = str(payload.get("alias") or "") or None
        entries.append(
            DeclaredTestEntry(
                kind=kind,
                stable_id=stable_id,
                alias=alias_text,
                assembly_name=str(payload.get("assemblyName") or payload.get("assembly_name") or ""),
                declaring_type=str(payload.get("declaringType") or payload.get("declaring_type") or ""),
                method_name=str(payload.get("methodName") or payload.get("method_name") or ""),
                method_signature=str(payload.get("methodSignature") or payload.get("method_signature") or ""),
                category=int(payload.get("category") or 0),
                metrics=int(payload.get("metrics") or 0),
                requires=int(payload.get("requires") or 0),
                evidence=int(payload.get("evidence") or 0),
                priority=int(payload.get("priority") or 0),
                warmup_count=int(payload.get("warmupCount") or payload.get("warmup_count") or 0),
                iteration_count=int(payload.get("iterationCount") or payload.get("iteration_count") or 0),
                invocation_count=int(payload.get("invocationCount") or payload.get("invocation_count") or 0),
            )
        )
    return entries


def test_declaration_mode(manifest: dict[str, Any]) -> TestDeclarationMode:
    raw_value = str(manifest.get("testDeclarationMode") or "").strip().lower()
    if not raw_value:
        return TestDeclarationMode.AUTO

    try:
        return TestDeclarationMode(raw_value)
    except ValueError as error:
        raise ValueError(f"unsupported testDeclarationMode: {raw_value}") from error


def stable_declared_test_id(
    *,
    subject_id: str,
    assembly_name: str,
    declaring_type: str,
    method_signature: str,
) -> str:
    required_parts = {
        "subject_id": subject_id,
        "assembly_name": assembly_name,
        "declaring_type": declaring_type,
        "method_signature": method_signature,
    }
    for field_name, value in required_parts.items():
        if not str(value).strip():
            raise ValueError(f"{field_name} is required")

    return "::".join(
        [
            subject_id.strip(),
            assembly_name.strip(),
            declaring_type.strip(),
            method_signature.strip(),
        ]
    )


def summarize_declaration_scan(
    *,
    mode: TestDeclarationMode,
    framework_referenced: bool,
    declared_unit_tests: Iterable[DeclaredTestEntry | dict[str, Any]] | None,
    declared_benchmarks: Iterable[DeclaredTestEntry | dict[str, Any]] | None,
) -> DeclarationScanSummary:
    normalized_unit_tests = _normalize_declared_entries(
        declared_unit_tests,
        kind=DeclaredTestKind.UNIT,
    )
    normalized_benchmarks = _normalize_declared_entries(
        declared_benchmarks,
        kind=DeclaredTestKind.BENCHMARK,
    )
    has_declarations = bool(normalized_unit_tests or normalized_benchmarks)
    warning_codes: list[DeclarationWarningCode] = []

    if mode is TestDeclarationMode.NONE:
        if has_declarations:
            raise ValueError("testDeclarationMode=none does not allow declared tests")
        return DeclarationScanSummary(
            mode=mode,
            framework_referenced=framework_referenced,
            declared_unit_tests=[],
            declared_benchmarks=[],
            subject_kind=SubjectDeclarationKind.RUNTIME_ONLY,
            warning_codes=[],
        )

    if mode is TestDeclarationMode.REQUIRE and not framework_referenced:
        raise ValueError("testDeclarationMode=require requires Chaos.TestFramework to be referenced")

    if mode is TestDeclarationMode.REQUIRE and not has_declarations:
        raise ValueError("testDeclarationMode=require requires at least one declared test entry")

    if not framework_referenced:
        return DeclarationScanSummary(
            mode=mode,
            framework_referenced=False,
            declared_unit_tests=[],
            declared_benchmarks=[],
            subject_kind=SubjectDeclarationKind.RUNTIME_ONLY,
            warning_codes=[],
        )

    if not has_declarations:
        warning_codes.append(DeclarationWarningCode.FRAMEWORK_WITHOUT_DECLARATIONS)
        return DeclarationScanSummary(
            mode=mode,
            framework_referenced=True,
            declared_unit_tests=[],
            declared_benchmarks=[],
            subject_kind=SubjectDeclarationKind.RUNTIME_ONLY,
            warning_codes=warning_codes,
        )

    return DeclarationScanSummary(
        mode=mode,
        framework_referenced=True,
        declared_unit_tests=normalized_unit_tests,
        declared_benchmarks=normalized_benchmarks,
        subject_kind=SubjectDeclarationKind.DECLARED_TEST,
        warning_codes=[],
    )
