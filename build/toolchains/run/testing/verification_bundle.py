from __future__ import annotations

from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import read_json, write_json
    from . import inventory_generator as inventory_generator_module
    from . import verification_codegen_stubs as verification_codegen_stubs_module
    from . import verification_contracts as verification_contracts_module
    from . import verification_merge as verification_merge_module
    from . import verification_reporting as verification_reporting_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import read_json, write_json
    from testing import inventory_generator as inventory_generator_module
    from testing import verification_codegen_stubs as verification_codegen_stubs_module
    from testing import verification_contracts as verification_contracts_module
    from testing import verification_merge as verification_merge_module
    from testing import verification_reporting as verification_reporting_module


UNIT_STAGE_ROUTE = {
    "collector": "managed",
    "registry": "managed",
    "workspace": "managed",
    "managed-proof": "managed",
    "native-proof": "native",
    "hotupdate-proof": "hotupdate",
}

UNIT_STAGE_EVIDENCE_KIND = {
    "collector": "contract",
    "registry": "contract",
    "workspace": "contract",
    "managed-proof": "semantic",
    "native-proof": "semantic",
    "hotupdate-proof": "semantic",
}


def _string(value: Any) -> str:
    return str(value or "")


def _string_list(values: Any) -> list[str]:
    if values in (None, ""):
        return []
    if isinstance(values, (list, tuple)):
        return [str(value) for value in values if str(value)]
    return [str(values)]


def _dedupe(values: list[str]) -> list[str]:
    ordered: list[str] = []
    seen: set[str] = set()
    for value in values:
        text = _string(value)
        if not text or text in seen:
            continue
        seen.add(text)
        ordered.append(text)
    return ordered


def _capability_id(row: dict[str, Any]) -> str:
    return f"capability/{int(row.get('capabilityFamily') or 0)}/{int(row.get('capabilityItem') or 0)}"


def _obligation_claim_id(row: dict[str, Any]) -> str:
    return f"obligation/{int(row.get('capabilityFamily') or 0)}/{int(row.get('capabilityItem') or 0)}"


def _device_profile_code(row: dict[str, Any]) -> str:
    explicit = _string(row.get("deviceProfileCode"))
    if explicit:
        return explicit
    device_id = _string(row.get("deviceId"))
    platform_id = _string(row.get("platformId")) or "default-platform"
    if device_id:
        return f"bridge/{platform_id}/{device_id}"
    return f"bridge/{platform_id}/default"


def _blocker(code: str, lane: str, message: str) -> dict[str, str]:
    return {
        "code": _string(code),
        "lane": _string(lane),
        "message": _string(message),
    }


def _aggregate_verification(states: list[str]) -> str:
    filtered = [state for state in (_string(value) for value in states) if state]
    if not filtered:
        return "planned"
    if any(state == "failed" for state in filtered):
        return "failed"
    if any(state == "blocked" for state in filtered):
        return "blocked"
    if any(state == "missing" for state in filtered):
        return "missing"
    if all(state == "not_required" for state in filtered):
        return "not_required"
    if all(state in {"passed", "not_required"} for state in filtered) and any(
        state == "passed" for state in filtered
    ):
        return "passed"
    if any(state == "planned" for state in filtered):
        return "planned"
    return filtered[-1]


def _aggregate_support(states: list[str]) -> str:
    filtered = [state for state in (_string(value) for value in states) if state]
    if not filtered:
        return "supported"
    active = [state for state in filtered if state != "not_applicable"]
    if not active:
        return "not_applicable"
    unique = set(active)
    if unique == {"supported"}:
        return "supported"
    if unique == {"unsupported"}:
        return "unsupported"
    return "partial"


def _mandatory_lanes(routes: list[str], evidence_kinds: list[str]) -> list[str]:
    lanes = ["test_governance"]
    if evidence_kinds:
        lanes.append("contracts")
    if any(route in {"native", "aot", "hotupdate"} for route in routes):
        lanes.extend(["aot", "il2cpp", "codegen"])
    if any(route == "hotupdate" for route in routes):
        lanes.append("loader")
    return _dedupe(lanes)


def _projection_meta(scope: str, row: dict[str, Any], row_key: str) -> dict[str, Any]:
    payload = dict(row)
    payload["scope"] = _string(scope)
    payload["rowKey"] = _string(row_key)
    return payload


def _unit_claim(row: dict[str, Any]) -> tuple[str, dict[str, Any]]:
    stage = _string(row.get("stage"))
    stage_status = _string(row.get("stageStatus"))
    stage_requirement = _string(row.get("stageRequirement"))
    support_state = {
        "unsupported": "unsupported",
        "not-applicable": "not_applicable",
    }.get(stage_requirement, "supported")
    verification_state = {
        "covered": "passed",
        "missing-evidence": "missing",
        "required": "planned",
        "optional": "planned",
        "unsupported": "blocked",
        "not-applicable": "not_required",
    }.get(stage_status, "planned")
    blockers: list[dict[str, str]] = []
    if verification_state == "missing":
        blockers.append(_blocker("missing-evidence", "contracts", _string(row.get("statusReason"))))
    elif verification_state == "blocked":
        blockers.append(_blocker("unsupported-stage", "test_governance", _string(row.get("statusReason"))))

    evidence_claim_id = f"evidence-claim/unit/{_string(row.get('stableId'))}/{stage}"
    projection_meta = _projection_meta(
        "unitTest",
        row,
        f"unit/{_string(row.get('stableId'))}/{stage}",
    )
    return stage, verification_contracts_module.build_evidence_claim_record(
        evidence_claim_id=evidence_claim_id,
        obligation_claim_id=_obligation_claim_id(row),
        scenario_id=_string(row.get("stableId")),
        scenario_kind="unit-test",
        route_code=UNIT_STAGE_ROUTE.get(stage, "managed"),
        platform_code="inventory-host",
        device_profile_code="inventory-host",
        evidence_kind=UNIT_STAGE_EVIDENCE_KIND.get(stage, "contract"),
        execution_contract_id=f"inventory-bridge/{stage}",
        verification_state=verification_state,
        support_state=support_state,
        blockers=blockers,
        stage_kind=stage,
        projection_meta=projection_meta,
    )


def _benchmark_claims(row: dict[str, Any]) -> list[tuple[str, dict[str, Any]]]:
    claims: list[tuple[str, dict[str, Any]]] = []
    device_profile_code = _device_profile_code(row)
    projection_meta = _projection_meta(
        "benchmark",
        {
            **dict(row),
            "deviceProfileCode": device_profile_code,
        },
        f"benchmark/{_string(row.get('stableId'))}/{device_profile_code}",
    )
    for route in _string_list(row.get("supportedModes")):
        status_key = f"{route}Status"
        status = _string(row.get(status_key))
        support_state = "unsupported" if status == "unsupported" else "supported"
        if status == "recorded" and bool(row.get("isStale")):
            verification_state = "missing"
        else:
            verification_state = {
                "recorded": "passed",
                "missing": "missing",
                "unsupported": "blocked",
            }.get(status, "planned")
        blockers: list[dict[str, str]] = []
        if verification_state == "missing":
            blockers.append(_blocker("missing-benchmark-record", "test_governance", f"{route} benchmark is missing."))
        elif verification_state == "blocked":
            blockers.append(_blocker("unsupported-benchmark-mode", "test_governance", f"{route} benchmark mode is unsupported."))
        evidence_claim_id = f"evidence-claim/benchmark/{_string(row.get('stableId'))}/{route}/{device_profile_code}"
        claims.append(
            (
                "perf",
                verification_contracts_module.build_evidence_claim_record(
                    evidence_claim_id=evidence_claim_id,
                    obligation_claim_id=_obligation_claim_id(row),
                    scenario_id=_string(row.get("stableId")),
                    scenario_kind="benchmark",
                    route_code=route,
                    platform_code=_string(row.get("platformId")) or "unknown-platform",
                    device_profile_code=device_profile_code,
                    evidence_kind="perf",
                    execution_contract_id=f"inventory-bridge/perf/{route}",
                    verification_state=verification_state,
                    support_state=support_state,
                    blockers=blockers,
                    stage_kind="perf",
                    projection_meta=projection_meta,
                ),
            )
        )
    return claims


def _contract_status_blockers(row: dict[str, Any]) -> list[dict[str, str]]:
    status = _string(row.get("contractStatus"))
    reason = _string(row.get("statusReason"))
    if status == "unsupported":
        return [_blocker("unsupported-capability", "test_governance", reason)]
    if status == "missingBenchmark":
        return [_blocker("missing-benchmark", "test_governance", reason)]
    if status == "missingProof":
        return [_blocker("missing-proof", "test_governance", reason)]
    return []


def _contract_support_state(row: dict[str, Any]) -> str:
    return "unsupported" if _string(row.get("contractStatus")) == "unsupported" else "supported"


def _contract_verification_state(row: dict[str, Any]) -> str:
    status = _string(row.get("contractStatus"))
    if status == "unsupported":
        return "blocked"
    if status in {"missingProof", "missingBenchmark"}:
        return "missing"
    return "planned"


def _build_stage_reports(
    evidence_claims_by_stage: dict[str, list[dict[str, Any]]],
) -> list[dict[str, Any]]:
    stage_reports: list[dict[str, Any]] = []
    for stage_kind, claims in sorted(evidence_claims_by_stage.items()):
        verification_states = [_string(item.get("verificationState")) for item in claims]
        support_states = [_string(item.get("supportState")) for item in claims]
        stage_reports.append(
            verification_contracts_module.build_stage_report_record(
                stage_id=stage_kind,
                closure_id="",
                scope_code="",
                stage_kind=stage_kind,
                verification_state=_aggregate_verification(verification_states),
                support_state=_aggregate_support(support_states),
                covered_obligation_claim_ids=_dedupe(
                    [_string(item.get("obligationClaimId")) for item in claims]
                ),
                success_evidence_ids=_dedupe(
                    [_string(item.get("evidenceClaimId")) for item in claims if _string(item.get("verificationState")) == "passed"]
                ),
                failed_evidence_ids=_dedupe(
                    [_string(item.get("evidenceClaimId")) for item in claims if _string(item.get("verificationState")) == "failed"]
                ),
                blocked_evidence_ids=_dedupe(
                    [_string(item.get("evidenceClaimId")) for item in claims if _string(item.get("verificationState")) == "blocked"]
                ),
                missing_evidence_claim_ids=_dedupe(
                    [_string(item.get("evidenceClaimId")) for item in claims if _string(item.get("verificationState")) == "missing"]
                ),
                blockers=[
                    dict(blocker)
                    for item in claims
                    for blocker in list(item.get("blockers") or [])
                ],
            )
        )
    return stage_reports


def _build_codegen_stub_indexes(source_payload: dict[str, Any]) -> list[dict[str, Any]]:
    stub_indexes: list[dict[str, Any]] = []
    for raw in list(source_payload.get("codegenStubs") or []):
        item = dict(raw or {})
        owner_subject_id = _string(item.get("ownerSubjectId"))
        capability_id = _string(item.get("capabilityId"))
        route_code = _string(item.get("routeCode"))
        if not owner_subject_id or not capability_id or not route_code:
            continue
        stub_indexes.append(
            verification_codegen_stubs_module.build_stub_index(
                owner_subject_id=owner_subject_id,
                capability_id=capability_id,
                route_code=route_code,
                profile_code=_string(item.get("profileCode")) or None,
                managed_source_refs=list(item.get("managedSourceRefs") or []),
                stub_refs=list(item.get("stubRefs") or []),
                generated_from_evidence_ids=_string_list(item.get("generatedFromEvidenceIds")),
            )
        )
    return stub_indexes


def build_verification_bundle(
    source_payload: dict[str, Any],
    *,
    closure_kind: str = "completed",
    scope_code: str = "testing-inventory",
) -> dict[str, Any]:
    inventory_outputs = inventory_generator_module.build_source_inventory_tables(source_payload)
    capability_rows = list(dict(inventory_outputs.get("capability") or {}).get("rows") or [])
    unit_rows = list(dict(inventory_outputs.get("unitTest") or {}).get("rows") or [])
    benchmark_rows = list(dict(inventory_outputs.get("benchmark") or {}).get("rows") or [])

    closure_id = f"closure/{_string(closure_kind)}/{_string(scope_code)}"
    evidence_claims_by_obligation: dict[str, list[dict[str, Any]]] = {}
    evidence_claims_by_stage: dict[str, list[dict[str, Any]]] = {}
    evidence_claim_items: list[dict[str, Any]] = []

    for row in unit_rows:
        stage_kind, claim = _unit_claim(dict(row))
        obligation_claim_id = _string(claim.get("obligationClaimId"))
        evidence_claims_by_obligation.setdefault(obligation_claim_id, []).append(claim)
        evidence_claims_by_stage.setdefault(stage_kind, []).append(claim)
        evidence_claim_items.append(claim)

    for row in benchmark_rows:
        for stage_kind, claim in _benchmark_claims(dict(row)):
            obligation_claim_id = _string(claim.get("obligationClaimId"))
            evidence_claims_by_obligation.setdefault(obligation_claim_id, []).append(claim)
            evidence_claims_by_stage.setdefault(stage_kind, []).append(claim)
            evidence_claim_items.append(claim)

    result_items: list[dict[str, Any]] = []
    capability_items: list[dict[str, Any]] = []
    for row in capability_rows:
        capability_row = dict(row)
        obligation_claim_id = _obligation_claim_id(capability_row)
        capability_id = _capability_id(capability_row)
        evidence_claims = list(evidence_claims_by_obligation.get(obligation_claim_id) or [])
        claim_verification_states = [_string(item.get("verificationState")) for item in evidence_claims]
        claim_support_states = [_string(item.get("supportState")) for item in evidence_claims]
        blockers = _contract_status_blockers(capability_row)
        verification_state = _aggregate_verification(
            [_contract_verification_state(capability_row)] + claim_verification_states
        )
        support_state = _aggregate_support([_contract_support_state(capability_row)] + claim_support_states)
        routes = _dedupe([_string(item.get("routeCode")) for item in evidence_claims])
        platforms = _dedupe([_string(item.get("platformCode")) for item in evidence_claims])
        device_profiles = _dedupe([_string(item.get("deviceProfileCode")) for item in evidence_claims])
        evidence_kinds = _dedupe([_string(item.get("evidenceKind")) for item in evidence_claims] + ["contract"])
        obligation = verification_contracts_module.build_obligation_claim_record(
            obligation_claim_id=obligation_claim_id,
            closure_id=closure_id,
            capability_id=capability_id,
            owner_subject_id=_string(capability_row.get("ownerSubjectId")),
            required_evidence_kinds=evidence_kinds,
            required_routes=routes,
            required_platforms=platforms,
            required_device_profiles=device_profiles,
            mandatory_lanes=_mandatory_lanes(routes, evidence_kinds),
            support_state=support_state,
            verification_state=verification_state,
            blockers=blockers,
            evidence_claim_ids=[_string(item.get("evidenceClaimId")) for item in evidence_claims],
            latest_stage_id=_string(evidence_claims[-1].get("stageKind")) if evidence_claims else "",
            capability_family=_string(capability_row.get("capabilityFamily")),
            projection_meta=_projection_meta("capability", capability_row, capability_id),
        )
        result_items.append(obligation)
        capability_items.append(
            verification_contracts_module.build_capability_summary_record(
                capability_id=capability_id,
                display_name=_string(capability_row.get("capabilityItemLabel")),
                owner_subject_id=_string(capability_row.get("ownerSubjectId")),
                default_routes=routes,
                default_platforms=platforms,
                default_device_profiles=device_profiles,
                required_evidence_kinds=evidence_kinds,
                verification_state=verification_state,
                support_state=support_state,
                blockers=blockers,
                latest_closure_id=closure_id,
                latest_obligation_claim_ids=[obligation_claim_id],
                projection_meta=_projection_meta("capability", capability_row, capability_id),
            )
        )

    stage_reports = _build_stage_reports(evidence_claims_by_stage)
    for report in stage_reports:
        report["closureId"] = closure_id
        report["scopeCode"] = _string(scope_code)

    closure = verification_contracts_module.build_closure_record(
        closure_id=closure_id,
        closure_kind=_string(closure_kind),
        scope_code=_string(scope_code),
        template_id=f"template/{_string(closure_kind)}-default",
        overlay_id=f"overlay/{_string(scope_code)}",
        target_capabilities=[_string(item.get("capabilityId")) for item in capability_items],
        required_routes=_dedupe(
            [route for item in result_items for route in _string_list(item.get("requiredRoutes"))]
        ),
        required_platforms=_dedupe(
            [platform for item in result_items for platform in _string_list(item.get("requiredPlatforms"))]
        ),
        required_device_profiles=_dedupe(
            [profile for item in result_items for profile in _string_list(item.get("requiredDeviceProfiles"))]
        ),
        required_evidence_kinds=_dedupe(
            [kind for item in result_items for kind in _string_list(item.get("requiredEvidenceKinds"))]
        ),
        mandatory_lanes=_dedupe(
            [lane for item in result_items for lane in _string_list(item.get("mandatoryLanes"))]
        ),
        verification_state=_aggregate_verification(
            [_string(item.get("verificationState")) for item in result_items]
        ),
        support_state=_aggregate_support([_string(item.get("supportState")) for item in result_items]),
        blockers=[dict(blocker) for item in result_items for blocker in list(item.get("blockers") or [])],
        obligation_count=len(result_items),
        passed_count=sum(1 for item in result_items if _string(item.get("verificationState")) == "passed"),
        failed_count=sum(1 for item in result_items if _string(item.get("verificationState")) == "failed"),
        blocked_count=sum(1 for item in result_items if _string(item.get("verificationState")) == "blocked"),
        missing_count=sum(1 for item in result_items if _string(item.get("verificationState")) == "missing"),
        not_required_count=sum(
            1 for item in result_items if _string(item.get("verificationState")) == "not_required"
        ),
    )

    latest = {
        "closure": verification_contracts_module.build_snapshot([closure], closure_id=closure_id),
        "capability": verification_contracts_module.build_snapshot(capability_items, closure_id=closure_id),
        "evidenceClaim": verification_contracts_module.build_snapshot(evidence_claim_items, closure_id=closure_id),
        "stage": verification_contracts_module.build_snapshot(stage_reports, closure_id=closure_id),
        "result": verification_contracts_module.build_snapshot(result_items, closure_id=closure_id),
    }
    master = {
        "closure": verification_contracts_module.build_master([closure], merge_key="closureId"),
        "capability": verification_contracts_module.build_master(capability_items, merge_key="capabilityId"),
        "evidenceClaim": verification_contracts_module.build_master(
            evidence_claim_items,
            merge_key="evidenceClaimId",
        ),
        "stage": verification_contracts_module.build_master(stage_reports, merge_key="stageId+scopeCode"),
        "result": verification_contracts_module.build_master(result_items, merge_key="obligationClaimId"),
    }
    return {
        "closureKind": _string(closure_kind),
        "scopeCode": _string(scope_code),
        "records": {
            "closure": [closure],
            "capability": capability_items,
            "evidenceClaims": evidence_claim_items,
            "stageReports": stage_reports,
            "result": result_items,
        },
        "codegenStubs": _build_codegen_stub_indexes(source_payload),
        "latest": latest,
        "master": master,
    }


def _relative(repo_root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _load_existing(path: Path) -> dict[str, Any] | None:
    if not path.is_file():
        return None
    return dict(read_json(path) or {})


def write_verification_bundle(
    repo_root: Path,
    source_payload: dict[str, Any],
    *,
    closure_kind: str = "completed",
    scope_code: str = "testing-inventory",
    output_root: Path | None = None,
    bundle: dict[str, Any] | None = None,
) -> dict[str, Any]:
    current_bundle = bundle or build_verification_bundle(
        source_payload,
        closure_kind=closure_kind,
        scope_code=scope_code,
    )
    root = output_root or repo_root / "docs" / "testing-inventory" / "verification"
    latest_root = root / "latest"
    master_root = root / "master"
    latest_root.mkdir(parents=True, exist_ok=True)
    master_root.mkdir(parents=True, exist_ok=True)

    report_paths = verification_reporting_module.write_stage_reports(
        repo_root=repo_root,
        closure_kind=_string(closure_kind),
        scope_code=_string(scope_code),
        stage_reports=current_bundle["records"]["stageReports"],
    )

    latest_files = {
        "closure": latest_root / "closure-snapshot.json",
        "capability": latest_root / "capability-snapshot.json",
        "evidenceClaim": latest_root / "evidence-claims-snapshot.json",
        "stage": latest_root / "stage-snapshot.json",
        "result": latest_root / "result-snapshot.json",
    }
    master_files = {
        "closure": master_root / "closure-master.json",
        "capability": master_root / "capability-master.json",
        "evidenceClaim": master_root / "evidence-claims-master.json",
        "stage": master_root / "stage-master.json",
        "result": master_root / "result-master.json",
    }

    for key, path in latest_files.items():
        write_json(path, current_bundle["latest"][key])

    closure_master = verification_merge_module.merge_closure_master(
        _load_existing(master_files["closure"]),
        current_bundle["master"]["closure"],
    )
    capability_master = verification_merge_module.merge_capability_master(
        _load_existing(master_files["capability"]),
        current_bundle["master"]["capability"],
    )
    evidence_claim_master = verification_merge_module.merge_evidence_claim_master(
        _load_existing(master_files["evidenceClaim"]),
        current_bundle["master"]["evidenceClaim"],
    )
    stage_master = verification_merge_module.merge_stage_master(
        _load_existing(master_files["stage"]),
        current_bundle["master"]["stage"],
    )
    result_master = verification_merge_module.merge_result_master(
        _load_existing(master_files["result"]),
        current_bundle["master"]["result"],
    )

    write_json(master_files["closure"], closure_master)
    write_json(master_files["capability"], capability_master)
    write_json(master_files["evidenceClaim"], evidence_claim_master)
    write_json(master_files["stage"], stage_master)
    write_json(master_files["result"], result_master)
    codegen_stub_paths = [
        verification_codegen_stubs_module.write_stub_index(repo_root, stub_index)
        for stub_index in list(current_bundle.get("codegenStubs") or [])
    ]

    artifacts = [
        _relative(repo_root, path)
        for path in list(latest_files.values()) + list(master_files.values())
    ]
    artifacts.append(report_paths["summaryPath"])
    artifacts.extend(report_paths["stagePaths"].values())
    artifacts.extend(_relative(repo_root, path) for path in codegen_stub_paths)

    return {
        "outputRoot": _relative(repo_root, root),
        "artifacts": artifacts,
        "closureId": current_bundle["latest"]["closure"]["closureId"],
        "tables": {
            "closure": {"rowCount": len(current_bundle["latest"]["closure"]["items"])},
            "capability": {"rowCount": len(current_bundle["latest"]["capability"]["items"])},
            "evidenceClaim": {"rowCount": len(current_bundle["latest"]["evidenceClaim"]["items"])},
            "stage": {"rowCount": len(current_bundle["latest"]["stage"]["items"])},
            "result": {"rowCount": len(current_bundle["latest"]["result"]["items"])},
        },
        "latestPayloads": dict(current_bundle["latest"]),
        "masterPayloads": {
            "closure": closure_master,
            "capability": capability_master,
            "evidenceClaim": evidence_claim_master,
            "stage": stage_master,
            "result": result_master,
        },
    }
