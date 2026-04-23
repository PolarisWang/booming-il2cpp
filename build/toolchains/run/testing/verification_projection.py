from __future__ import annotations

from typing import Any


_LIST_FIELDS = {
    "supportStates",
    "supportStateLabels",
    "supportedModes",
    "metricLabels",
    "evidenceLabels",
}

_STATUS_KEYS = ("passed", "failed", "blocked", "missing", "planned", "not_required")
_STATUS_ORDER = {
    "failed": 0,
    "blocked": 1,
    "missing": 2,
    "planned": 3,
    "passed": 4,
    "not_required": 5,
}
_ROUTE_ORDER = {
    "managed": 0,
    "native": 1,
    "hotupdate": 2,
    "interpreter": 3,
}
_STAGE_ORDER = {
    "collector": 10,
    "registry": 20,
    "workspace": 30,
    "managed-proof": 40,
    "native-proof": 50,
    "hotupdate-proof": 60,
    "perf": 70,
}


def _string(value: Any) -> str:
    return str(value or "")


def _int(value: Any) -> int:
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _list(values: Any) -> list[Any]:
    if isinstance(values, list):
        return list(values)
    if isinstance(values, tuple):
        return list(values)
    if values in (None, ""):
        return []
    return [values]


def _has_value(value: Any) -> bool:
    return value is not None and value != "" and value != [] and value != {}


def _merge_list(existing: Any, incoming: Any) -> list[Any]:
    merged: list[Any] = []
    seen: set[str] = set()
    for raw in _list(existing) + _list(incoming):
        key = repr(raw)
        if key in seen:
            continue
        seen.add(key)
        merged.append(raw)
    return merged


def _clean_projection_row(meta: dict[str, Any]) -> dict[str, Any]:
    row = dict(meta or {})
    row.pop("scope", None)
    row.pop("rowKey", None)
    return row


def _merge_projection_rows(base: dict[str, Any], incoming: dict[str, Any]) -> dict[str, Any]:
    if not base:
        return dict(incoming or {})
    merged = dict(base)
    for key, value in dict(incoming or {}).items():
        if key in _LIST_FIELDS:
            merged[key] = _merge_list(merged.get(key), value)
            continue
        if _has_value(value) or key not in merged:
            merged[key] = value
    return merged


def _items(formal_source: dict[str, Any], key: str) -> list[dict[str, Any]]:
    direct = dict(formal_source or {}).get(key)
    if isinstance(direct, dict) and "items" in direct:
        return [dict(item or {}) for item in list(direct.get("items") or [])]

    master = dict(dict(formal_source or {}).get("master") or {})
    master_entry = master.get(key)
    if isinstance(master_entry, dict) and "items" in master_entry:
        return [dict(item or {}) for item in list(master_entry.get("items") or [])]

    records = dict(dict(formal_source or {}).get("records") or {})
    record_key = {
        "evidenceClaim": "evidenceClaims",
    }.get(key, key)
    if isinstance(records.get(record_key), list):
        return [dict(item or {}) for item in list(records.get(record_key) or [])]
    return []


def _status_column(status: str) -> str:
    normalized = _string(status) or "planned"
    if normalized not in _STATUS_KEYS:
        normalized = "planned"
    return f"{normalized}Count"


def _increment_status(row: dict[str, Any], status: str) -> None:
    row[_status_column(status)] = _int(row.get(_status_column(status))) + 1


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


def _attention_count(row: dict[str, Any]) -> int:
    return (
        _int(row.get("failedCount"))
        + _int(row.get("blockedCount"))
        + _int(row.get("missingCount"))
        + _int(row.get("plannedCount"))
    )


def _sample(values: list[str], *, limit: int = 3) -> list[str]:
    ordered: list[str] = []
    seen: set[str] = set()
    for raw in values:
        text = _string(raw)
        if not text or text in seen:
            continue
        seen.add(text)
        ordered.append(text)
        if len(ordered) >= limit:
            break
    return ordered


def _summarize_counts(values: list[str], *, order: dict[str, int] | None = None) -> list[str]:
    counts: dict[str, int] = {}
    for raw in values:
        text = _string(raw)
        if not text:
            continue
        counts[text] = counts.get(text, 0) + 1
    return [
        f"{key} x{counts[key]}"
        for key in sorted(counts, key=lambda item: (order.get(item, 999) if order else 999, item))
    ]


def _capability_rows(formal_source: dict[str, Any]) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    for item in _items(formal_source, "capability"):
        meta = _clean_projection_row(dict(item.get("projectionMeta") or {}))
        if meta:
            rows.append(meta)
            continue
        rows.append(
            {
                "ownerSubjectId": _string(item.get("ownerSubjectId")),
                "capabilityFamily": 0,
                "capabilityFamilyLabel": "",
                "capabilityItem": 0,
                "capabilityItemLabel": _string(item.get("displayName")),
                "supportStates": [],
                "supportStateLabels": [],
                "proofRequired": False,
                "benchmarkRequired": False,
                "declaredUnitCount": 0,
                "declaredBenchmarkCount": 0,
                "contractStatus": "",
                "statusReason": "",
            }
        )
    return sorted(
        rows,
        key=lambda row: (
            _int(row.get("capabilityFamily")),
            _int(row.get("capabilityItem")),
            _string(row.get("ownerSubjectId")),
        ),
    )


def _unit_rows(formal_source: dict[str, Any]) -> list[dict[str, Any]]:
    rows_by_key: dict[str, dict[str, Any]] = {}
    for item in _items(formal_source, "evidenceClaim"):
        if _string(item.get("scenarioKind")) != "unit-test":
            continue
        meta = _clean_projection_row(dict(item.get("projectionMeta") or {}))
        row_key = _string(dict(item.get("projectionMeta") or {}).get("rowKey")) or _string(item.get("evidenceClaimId"))
        if not meta:
            meta = {
                "subjectId": "",
                "stableId": _string(item.get("scenarioId")),
                "alias": "",
                "method": _string(item.get("scenarioId")),
                "stage": _string(item.get("stageKind")),
                "stageOrder": 0,
                "capabilityFamily": 0,
                "capabilityItem": 0,
                "capabilityItemLabel": "",
                "ownerSubjectId": "",
                "supportStateLabels": [],
                "stageRequirement": "",
                "stageCoverage": "",
                "stageStatus": "",
                "statusReason": "",
                "defaultGoalId": "",
                "defaultMatrixId": "",
                "evidenceLabels": [],
                "priority": 0,
            }
        rows_by_key[row_key] = _merge_projection_rows(rows_by_key.get(row_key, {}), meta)
    return sorted(
        rows_by_key.values(),
        key=lambda row: (
            _string(row.get("stableId")),
            _int(row.get("stageOrder")),
            _string(row.get("stage")),
        ),
    )


def _benchmark_rows(formal_source: dict[str, Any]) -> list[dict[str, Any]]:
    rows_by_key: dict[str, dict[str, Any]] = {}
    for item in _items(formal_source, "evidenceClaim"):
        if _string(item.get("scenarioKind")) != "benchmark":
            continue
        meta = _clean_projection_row(dict(item.get("projectionMeta") or {}))
        row_key = _string(dict(item.get("projectionMeta") or {}).get("rowKey"))
        if not row_key:
            row_key = "|".join(
                [
                    _string(item.get("scenarioId")),
                    _string(item.get("platformCode")),
                    _string(item.get("deviceProfileCode")),
                ]
            )
        if not meta:
            meta = {
                "platformId": _string(item.get("platformCode")),
                "deviceId": _string(item.get("deviceProfileCode")),
                "deviceName": "",
                "subjectId": "",
                "stableId": _string(item.get("scenarioId")),
                "alias": "",
                "method": _string(item.get("scenarioId")),
                "capabilityFamily": 0,
                "capabilityItem": 0,
                "capabilityItemLabel": "",
                "ownerSubjectId": "",
                "supportStateLabels": [],
                "supportedModes": [],
                "metricLabels": [],
                "warmupCount": 0,
                "iterationCount": 0,
                "invocationCount": 0,
                "managedStatus": "",
                "managedMeanDurationMs": "",
                "managedOpsPerSecond": "",
                "nativeStatus": "",
                "nativeMeanDurationMs": "",
                "nativeOpsPerSecond": "",
                "nativeRelativeToManaged": "",
                "interpreterStatus": "",
                "interpreterMeanDurationMs": "",
                "interpreterOpsPerSecond": "",
                "interpreterRelativeToManaged": "",
                "isStale": False,
                "lastRecordedAt": "",
                "gitCommit": "",
            }
        rows_by_key[row_key] = _merge_projection_rows(rows_by_key.get(row_key, {}), meta)
    return sorted(
        rows_by_key.values(),
        key=lambda row: (
            _string(row.get("stableId")),
            _string(row.get("deviceId")),
            _string(row.get("platformId")),
        ),
    )


def _closure_view(formal_source: dict[str, Any]) -> dict[str, Any]:
    rows: list[dict[str, Any]] = []
    for item in _items(formal_source, "closure"):
        obligation_count = _int(item.get("obligationCount"))
        passed_count = _int(item.get("passedCount"))
        failed_count = _int(item.get("failedCount"))
        blocked_count = _int(item.get("blockedCount"))
        missing_count = _int(item.get("missingCount"))
        not_required_count = _int(item.get("notRequiredCount"))
        planned_count = max(
            0,
            obligation_count
            - passed_count
            - failed_count
            - blocked_count
            - missing_count
            - not_required_count,
        )
        rows.append(
            {
                "closureId": _string(item.get("closureId")),
                "closureKind": _string(item.get("closureKind")),
                "scopeCode": _string(item.get("scopeCode")),
                "verificationState": _string(item.get("verificationState")),
                "supportState": _string(item.get("supportState")),
                "obligationCount": obligation_count,
                "passedCount": passed_count,
                "failedCount": failed_count,
                "blockedCount": blocked_count,
                "missingCount": missing_count,
                "plannedCount": planned_count,
                "notRequiredCount": not_required_count,
                "requiredRoutes": _list(item.get("requiredRoutes")),
                "requiredPlatforms": _list(item.get("requiredPlatforms")),
                "requiredDeviceProfiles": _list(item.get("requiredDeviceProfiles")),
                "requiredEvidenceKinds": _list(item.get("requiredEvidenceKinds")),
            }
        )
    rows.sort(
        key=lambda row: (
            _STATUS_ORDER.get(_string(row.get("verificationState")), 999),
            _string(row.get("closureId")),
        )
    )
    chips: list[dict[str, str | int]] = []
    if rows:
        primary = rows[0]
        chips = [
            {"label": "Closure", "value": f"{primary['closureKind']} / {primary['scopeCode']}"},
            {"label": "Verification", "value": _string(primary.get("verificationState"))},
            {"label": "Support", "value": _string(primary.get("supportState"))},
            {"label": "Obligations", "value": _int(primary.get("obligationCount"))},
            {"label": "Attention", "value": _attention_count(primary)},
        ]
    return {
        "id": "closure-strip",
        "title": "Closure Strip",
        "description": "Current closure scope and overall formal status.",
        "chips": chips,
        "headers": [
            {"key": "closureId", "label": "Closure"},
            {"key": "closureKind", "label": "Kind"},
            {"key": "scopeCode", "label": "Scope"},
            {"key": "verificationState", "label": "Verification"},
            {"key": "supportState", "label": "Support"},
            {"key": "obligationCount", "label": "Obligations"},
            {"key": "passedCount", "label": "Passed"},
            {"key": "failedCount", "label": "Failed"},
            {"key": "blockedCount", "label": "Blocked"},
            {"key": "missingCount", "label": "Missing"},
            {"key": "plannedCount", "label": "Planned"},
            {"key": "requiredRoutes", "label": "Routes"},
            {"key": "requiredPlatforms", "label": "Platforms"},
            {"key": "requiredDeviceProfiles", "label": "DeviceProfiles"},
            {"key": "requiredEvidenceKinds", "label": "EvidenceKinds"},
        ],
        "rows": rows,
    }


def _responsibility_view(formal_source: dict[str, Any]) -> dict[str, Any]:
    rows_by_key: dict[str, dict[str, Any]] = {}
    for item in _items(formal_source, "result"):
        meta = dict(item.get("projectionMeta") or {})
        owner = _string(item.get("ownerSubjectId")) or _string(meta.get("ownerSubjectId"))
        lanes = [value for value in _list(item.get("mandatoryLanes")) if _string(value)] or ["unassigned"]
        capability_label = _string(meta.get("capabilityItemLabel")) or _string(item.get("capabilityId"))
        blockers = [dict(blocker or {}) for blocker in _list(item.get("blockers")) if isinstance(blocker, dict)]
        for lane in lanes:
            key = f"{owner}|{lane}"
            row = rows_by_key.setdefault(
                key,
                {
                    "ownerSubjectId": owner,
                    "lane": _string(lane),
                    "obligationCount": 0,
                    "passedCount": 0,
                    "failedCount": 0,
                    "blockedCount": 0,
                    "missingCount": 0,
                    "plannedCount": 0,
                    "notRequiredCount": 0,
                    "blockerCount": 0,
                    "_capabilitySamples": [],
                    "_blockerExamples": [],
                },
            )
            row["obligationCount"] = _int(row.get("obligationCount")) + 1
            _increment_status(row, _string(item.get("verificationState")))
            row["_capabilitySamples"].append(capability_label)
            row["blockerCount"] = _int(row.get("blockerCount")) + len(blockers)
            for blocker in blockers:
                message = _string(blocker.get("message")) or _string(blocker.get("code"))
                if message:
                    row["_blockerExamples"].append(message)
    rows: list[dict[str, Any]] = []
    for row in rows_by_key.values():
        rows.append(
            {
                "ownerSubjectId": _string(row.get("ownerSubjectId")),
                "lane": _string(row.get("lane")),
                "obligationCount": _int(row.get("obligationCount")),
                "passedCount": _int(row.get("passedCount")),
                "failedCount": _int(row.get("failedCount")),
                "blockedCount": _int(row.get("blockedCount")),
                "missingCount": _int(row.get("missingCount")),
                "plannedCount": _int(row.get("plannedCount")),
                "blockerCount": _int(row.get("blockerCount")),
                "capabilitySamples": _sample(list(row.get("_capabilitySamples") or [])),
                "blockerExamples": _sample(list(row.get("_blockerExamples") or [])),
            }
        )
    rows.sort(
        key=lambda row: (
            -_int(row.get("blockerCount")),
            -_attention_count(row),
            _string(row.get("ownerSubjectId")),
            _string(row.get("lane")),
        )
    )
    chips = [
        {"label": "Rows", "value": len(rows)},
        {"label": "Owners", "value": len({_string(row.get('ownerSubjectId')) for row in rows})},
        {"label": "Attention", "value": sum(_attention_count(row) for row in rows)},
    ]
    return {
        "id": "responsibility-view",
        "title": "Responsibility View",
        "description": "Owner subject and mainline lane coverage for the current closure.",
        "chips": chips,
        "headers": [
            {"key": "ownerSubjectId", "label": "Owner"},
            {"key": "lane", "label": "Lane"},
            {"key": "obligationCount", "label": "Obligations"},
            {"key": "passedCount", "label": "Passed"},
            {"key": "failedCount", "label": "Failed"},
            {"key": "blockedCount", "label": "Blocked"},
            {"key": "missingCount", "label": "Missing"},
            {"key": "plannedCount", "label": "Planned"},
            {"key": "blockerCount", "label": "Blockers"},
            {"key": "capabilitySamples", "label": "Capabilities"},
            {"key": "blockerExamples", "label": "Examples"},
        ],
        "rows": rows,
    }


def _route_topology_view(formal_source: dict[str, Any]) -> dict[str, Any]:
    rows_by_key: dict[str, dict[str, Any]] = {}
    for item in _items(formal_source, "evidenceClaim"):
        route = _string(item.get("routeCode")) or "unknown"
        meta = dict(item.get("projectionMeta") or {})
        row = rows_by_key.setdefault(
            route,
            {
                "routeCode": route,
                "claimCount": 0,
                "passedCount": 0,
                "failedCount": 0,
                "blockedCount": 0,
                "missingCount": 0,
                "plannedCount": 0,
                "notRequiredCount": 0,
                "_verificationStates": [],
                "_gapStages": [],
                "_owners": [],
            },
        )
        row["claimCount"] = _int(row.get("claimCount")) + 1
        status = _string(item.get("verificationState"))
        _increment_status(row, status)
        row["_verificationStates"].append(status)
        row["_owners"].append(_string(meta.get("ownerSubjectId")))
        if status not in {"passed", "not_required"}:
            row["_gapStages"].append(_string(item.get("stageKind")))
    rows: list[dict[str, Any]] = []
    for row in rows_by_key.values():
        verification_state = _aggregate_verification(list(row.get("_verificationStates") or []))
        rows.append(
            {
                "routeCode": _string(row.get("routeCode")),
                "verificationState": verification_state,
                "claimCount": _int(row.get("claimCount")),
                "passedCount": _int(row.get("passedCount")),
                "failedCount": _int(row.get("failedCount")),
                "blockedCount": _int(row.get("blockedCount")),
                "missingCount": _int(row.get("missingCount")),
                "plannedCount": _int(row.get("plannedCount")),
                "notRequiredCount": _int(row.get("notRequiredCount")),
                "gapStages": _summarize_counts(list(row.get("_gapStages") or []), order=_STAGE_ORDER),
                "owners": _sample(list(row.get("_owners") or []), limit=4),
            }
        )
    rows.sort(
        key=lambda row: (
            _ROUTE_ORDER.get(_string(row.get("routeCode")), 999),
            -_attention_count(row),
            _string(row.get("routeCode")),
        )
    )
    chips = [
        {"label": "Routes", "value": len(rows)},
        {"label": "Attention", "value": sum(_attention_count(row) for row in rows)},
    ]
    return {
        "id": "route-topology-view",
        "title": "Route Topology View",
        "description": "Route-level coverage from formal evidence claims.",
        "chips": chips,
        "headers": [
            {"key": "routeCode", "label": "Route"},
            {"key": "verificationState", "label": "Verification"},
            {"key": "claimCount", "label": "Claims"},
            {"key": "passedCount", "label": "Passed"},
            {"key": "failedCount", "label": "Failed"},
            {"key": "blockedCount", "label": "Blocked"},
            {"key": "missingCount", "label": "Missing"},
            {"key": "plannedCount", "label": "Planned"},
            {"key": "notRequiredCount", "label": "NotRequired"},
            {"key": "gapStages", "label": "GapStages"},
            {"key": "owners", "label": "Owners"},
        ],
        "rows": rows,
    }


def _platform_device_matrix_view(formal_source: dict[str, Any]) -> dict[str, Any]:
    rows_by_key: dict[str, dict[str, Any]] = {}
    for item in _items(formal_source, "evidenceClaim"):
        meta = dict(item.get("projectionMeta") or {})
        platform_code = _string(item.get("platformCode")) or "unknown-platform"
        device_profile_code = _string(item.get("deviceProfileCode")) or "unknown-device-profile"
        key = f"{platform_code}|{device_profile_code}"
        row = rows_by_key.setdefault(
            key,
            {
                "platformCode": platform_code,
                "deviceProfileCode": device_profile_code,
                "claimCount": 0,
                "passedCount": 0,
                "failedCount": 0,
                "blockedCount": 0,
                "missingCount": 0,
                "plannedCount": 0,
                "notRequiredCount": 0,
                "_routes": [],
                "_evidenceKinds": [],
                "_owners": [],
            },
        )
        row["claimCount"] = _int(row.get("claimCount")) + 1
        _increment_status(row, _string(item.get("verificationState")))
        row["_routes"].append(_string(item.get("routeCode")))
        row["_evidenceKinds"].append(_string(item.get("evidenceKind")))
        row["_owners"].append(_string(meta.get("ownerSubjectId")))
    rows: list[dict[str, Any]] = []
    for row in rows_by_key.values():
        rows.append(
            {
                "platformCode": _string(row.get("platformCode")),
                "deviceProfileCode": _string(row.get("deviceProfileCode")),
                "claimCount": _int(row.get("claimCount")),
                "passedCount": _int(row.get("passedCount")),
                "failedCount": _int(row.get("failedCount")),
                "blockedCount": _int(row.get("blockedCount")),
                "missingCount": _int(row.get("missingCount")),
                "plannedCount": _int(row.get("plannedCount")),
                "notRequiredCount": _int(row.get("notRequiredCount")),
                "routes": _sample(list(row.get("_routes") or []), limit=4),
                "evidenceKinds": _sample(list(row.get("_evidenceKinds") or []), limit=4),
                "owners": _sample(list(row.get("_owners") or []), limit=4),
            }
        )
    rows.sort(
        key=lambda row: (
            -_attention_count(row),
            _string(row.get("platformCode")),
            _string(row.get("deviceProfileCode")),
        )
    )
    chips = [
        {"label": "Profiles", "value": len(rows)},
        {"label": "Attention", "value": sum(_attention_count(row) for row in rows)},
    ]
    return {
        "id": "platform-device-matrix",
        "title": "Platform-Device Matrix",
        "description": "Platform and device-profile coverage projected from formal evidence.",
        "chips": chips,
        "headers": [
            {"key": "platformCode", "label": "Platform"},
            {"key": "deviceProfileCode", "label": "DeviceProfile"},
            {"key": "claimCount", "label": "Claims"},
            {"key": "passedCount", "label": "Passed"},
            {"key": "failedCount", "label": "Failed"},
            {"key": "blockedCount", "label": "Blocked"},
            {"key": "missingCount", "label": "Missing"},
            {"key": "plannedCount", "label": "Planned"},
            {"key": "notRequiredCount", "label": "NotRequired"},
            {"key": "routes", "label": "Routes"},
            {"key": "evidenceKinds", "label": "EvidenceKinds"},
            {"key": "owners", "label": "Owners"},
        ],
        "rows": rows,
    }


def project_homepage_views(formal_source: dict[str, Any]) -> list[dict[str, Any]]:
    return [
        _closure_view(formal_source),
        _responsibility_view(formal_source),
        _route_topology_view(formal_source),
        _platform_device_matrix_view(formal_source),
    ]


def project_inventory_tables(formal_source: dict[str, Any]) -> dict[str, list[dict[str, Any]]]:
    return {
        "capability": _capability_rows(formal_source),
        "unitTest": _unit_rows(formal_source),
        "benchmark": _benchmark_rows(formal_source),
    }
