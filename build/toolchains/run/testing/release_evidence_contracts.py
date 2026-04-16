from __future__ import annotations

from pathlib import Path
from typing import Any
import sys

try:
    from . import capability_coverage as capability_coverage_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import capability_coverage as capability_coverage_module


_FAIL_CLASSIFICATIONS = {"missingProof", "missingBenchmark", "unsupported"}


def _int_value(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _stable_id(payload: dict[str, Any]) -> str:
    return str(payload.get("stableId") or payload.get("id") or "")


def _empty_status_counts() -> dict[str, int]:
    return {
        "total": 0,
        "ok": 0,
        "fail": 0,
    }


def _empty_classification_counts() -> dict[str, int]:
    return {
        "total": 0,
        "ok": 0,
        "missingProof": 0,
        "missingBenchmark": 0,
        "proofOptional": 0,
        "unsupported": 0,
    }


def _normalized_declared_entry(payload: dict[str, Any], *, kind: str) -> dict[str, Any] | None:
    capability_item = _int_value(payload.get("capabilityItem"))
    if capability_item <= 0:
        return None

    contract = capability_coverage_module.resolve_capability_contract(
        capability_family=payload.get("capabilityFamily"),
        capability_item=capability_item,
    )
    owner_subject_id = str(contract.get("ownerSubjectId") or payload.get("subjectId") or "")
    subject_id = str(payload.get("subjectId") or owner_subject_id)
    return {
        "kind": kind,
        "subjectId": subject_id,
        "ownerSubjectId": owner_subject_id,
        "stableId": _stable_id(payload),
        "contract": contract,
    }


def _classification_payload(
    *,
    support_states: list[int],
    proof_required: bool,
    benchmark_required: bool,
    has_declared_proof: bool,
    has_declared_benchmark: bool,
) -> tuple[str, str, str]:
    if not proof_required and not benchmark_required:
        return (
            "proofOptional",
            "reporting-owned-proof-optional",
            "Proof is optional; evidence is owned by reporting and release gates.",
        )
    if 6 in support_states:
        return (
            "unsupported",
            "unsupported-by-contract",
            "Capability contract contains unsupported execution states.",
        )
    if proof_required and not has_declared_proof:
        return (
            "missingProof",
            "missing-proof-record",
            "Required proof declaration is missing for this capability contract.",
        )
    if benchmark_required and not has_declared_benchmark:
        return (
            "missingBenchmark",
            "missing-benchmark-record",
            "Required benchmark declaration is missing for this capability contract.",
        )
    return (
        "ok",
        "covered-by-declared-evidence",
        "Required declared proof and benchmark evidence are present.",
    )


def build_declared_contract_status_report(
    *,
    declared_unit_tests: list[dict[str, Any]] | None = None,
    declared_benchmarks: list[dict[str, Any]] | None = None,
) -> dict[str, Any]:
    grouped_contracts: dict[tuple[str, int, int], dict[str, Any]] = {}

    for payload in list(declared_unit_tests or []):
        normalized = _normalized_declared_entry(payload, kind="proof")
        if normalized is None:
            continue
        contract = dict(normalized["contract"])
        key = (
            str(normalized["ownerSubjectId"]),
            int(contract.get("capabilityFamily") or 0),
            int(contract.get("capabilityItem") or 0),
        )
        group = grouped_contracts.setdefault(
            key,
            {
                "ownerSubjectId": str(normalized["ownerSubjectId"]),
                "contract": contract,
                "proofEntries": [],
                "benchmarkEntries": [],
            },
        )
        group["proofEntries"].append(normalized)

    for payload in list(declared_benchmarks or []):
        normalized = _normalized_declared_entry(payload, kind="benchmark")
        if normalized is None:
            continue
        contract = dict(normalized["contract"])
        key = (
            str(normalized["ownerSubjectId"]),
            int(contract.get("capabilityFamily") or 0),
            int(contract.get("capabilityItem") or 0),
        )
        group = grouped_contracts.setdefault(
            key,
            {
                "ownerSubjectId": str(normalized["ownerSubjectId"]),
                "contract": contract,
                "proofEntries": [],
                "benchmarkEntries": [],
            },
        )
        group["benchmarkEntries"].append(normalized)

    status_counts = _empty_status_counts()
    classification_counts = _empty_classification_counts()
    contract_results: list[dict[str, Any]] = []
    subject_reports: dict[str, dict[str, Any]] = {}

    for key in sorted(grouped_contracts):
        group = grouped_contracts[key]
        contract = dict(group["contract"])
        proof_entries = list(group["proofEntries"])
        benchmark_entries = list(group["benchmarkEntries"])
        support_states = [int(value) for value in list(contract.get("supportStates") or [])]
        proof_required = bool(contract.get("proofRequired", False))
        benchmark_required = bool(contract.get("benchmarkRequired", False))
        has_declared_proof = bool(proof_entries)
        has_declared_benchmark = bool(benchmark_entries)
        classification, reason_code, reason_label = _classification_payload(
            support_states=support_states,
            proof_required=proof_required,
            benchmark_required=benchmark_required,
            has_declared_proof=has_declared_proof,
            has_declared_benchmark=has_declared_benchmark,
        )
        status = "fail" if classification in _FAIL_CLASSIFICATIONS else "ok"

        result = {
            "ownerSubjectId": str(group["ownerSubjectId"]),
            "declaredSubjectIds": sorted(
                {
                    str(entry["subjectId"])
                    for entry in [*proof_entries, *benchmark_entries]
                    if str(entry["subjectId"])
                }
            ),
            "capabilityFamily": int(contract.get("capabilityFamily") or 0),
            "capabilityFamilyLabel": str(contract.get("capabilityFamilyLabel") or ""),
            "capabilityItem": int(contract.get("capabilityItem") or 0),
            "capabilityItemLabel": str(contract.get("capabilityItemLabel") or ""),
            "supportStates": support_states,
            "supportStateLabels": [str(value) for value in list(contract.get("supportStateLabels") or [])],
            "proofRequired": proof_required,
            "benchmarkRequired": benchmark_required,
            "proofOptional": not proof_required,
            "reportingOwned": not proof_required and not benchmark_required,
            "hasDeclaredProof": has_declared_proof,
            "hasDeclaredBenchmark": has_declared_benchmark,
            "declaredProofCount": len(proof_entries),
            "declaredBenchmarkCount": len(benchmark_entries),
            "declaredProofStableIds": sorted(
                stable_id for stable_id in (_stable_id(entry) for entry in proof_entries) if stable_id
            ),
            "declaredBenchmarkStableIds": sorted(
                stable_id for stable_id in (_stable_id(entry) for entry in benchmark_entries) if stable_id
            ),
            "classification": classification,
            "reasonCode": reason_code,
            "reasonLabel": reason_label,
            "missingProof": classification == "missingProof",
            "missingBenchmark": classification == "missingBenchmark",
            "unsupported": classification == "unsupported",
            "status": status,
        }
        contract_results.append(result)

        status_counts["total"] += 1
        status_counts[status] += 1
        classification_counts["total"] += 1
        classification_counts[classification] += 1

        owner_subject_id = str(result["ownerSubjectId"])
        subject_report = subject_reports.setdefault(
            owner_subject_id,
            {
                "subjectId": owner_subject_id,
                "status": "ok",
                "statusCounts": _empty_status_counts(),
                "classificationCounts": _empty_classification_counts(),
                "contractResults": [],
            },
        )
        subject_report["statusCounts"]["total"] += 1
        subject_report["statusCounts"][status] += 1
        subject_report["classificationCounts"]["total"] += 1
        subject_report["classificationCounts"][classification] += 1
        subject_report["contractResults"].append(result)
        if status == "fail":
            subject_report["status"] = "fail"

    return {
        "reportVersion": "v1",
        "status": "fail" if status_counts["fail"] else "ok",
        "statusCounts": status_counts,
        "classificationCounts": classification_counts,
        "subjectResults": [subject_reports[key] for key in sorted(subject_reports)],
        "contractResults": contract_results,
    }
