from __future__ import annotations

import json
import re
import sys
from html import escape
from pathlib import Path
from typing import Any

try:
    from .foundation_dll import case_index_loader as case_index_loader_module
    from ..core.common import write_json
    from .foundation_dll import family_verification_claims as family_verification_claims_module
    from .foundation_dll import truth_contracts as truth_contracts_module
    from .foundation_dll import verification_kernel as verification_kernel_module
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing.foundation_dll import case_index_loader as case_index_loader_module
    from core.common import write_json
    from testing.foundation_dll import family_verification_claims as family_verification_claims_module
    from testing.foundation_dll import truth_contracts as truth_contracts_module
    from testing.foundation_dll import verification_kernel as verification_kernel_module
    from testing import verification_layout as verification_layout_module


PROGRAM_MANIFEST_RELATIVE_PATH = (
    "verification",
    "catalog",
    "programs",
    "foundation-dll-translation-audit.program.json",
)
ROADMAP_STATUS_RELATIVE_PATH = (
    "docs",
    "dev",
    "completed",
    "20260427-04-dll-capability-verification-closure-roadmap",
    "STATUS.md",
)
ROADMAP_PLAN_RELATIVE_PATH = (
    "docs",
    "dev",
    "completed",
    "20260427-04-dll-capability-verification-closure-roadmap",
    "roadmap-v1-01.md",
)
ROADMAP_PHASE_STATUS_OVERRIDES = {
    "System.Private.CoreLib": {
        "phase": "roadmap-closed",
        "dllState": "in-progress",
        "currentProject": "completion-certification",
        "blockingReason": "24 families all gates passed. Completion certification evidence generated. Remaining: full CoreLib coverage widening (54310 uncovered methods in supplemental skeleton).",
    }
}
ROADMAP_STATUS_TO_DLL_STATE = {
    "planned": "not-started",
    "in-progress": "in-progress",
    "blocked": "blocked",
    "completed": "completed",
    "completed-blocked": "blocked",
}
PROJECT_PROGRESS_ORDER = (
    "audit-input-and-ledger",
    "managed-proof",
    "native-proof",
    "hotupdate-proof",
    "benchmark",
    "codegen-review",
    "completion-certification",
)
ARTIFACT_PATH_PATTERN = re.compile(r"(?P<path>(?:artifacts|docs|verification|subjects)/[^\s`]+)")
LEDGER_RELATIVE_PATH = (
    "verification",
    "projections",
    "foundation-dll-audit",
    "capability-family-ledger.json",
)


def _string(value: Any) -> str:
    return str(value or "").strip()


def _list(value: Any) -> list[Any]:
    if isinstance(value, list):
        return list(value)
    if isinstance(value, tuple):
        return list(value)
    if value in (None, ""):
        return []
    return [value]


def _relative(repo_root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _normalized(text: str) -> str:
    return str(text or "").replace("\\", "/")


def _safe_display_text(value: Any, fallback: str) -> str:
    text = _string(value)
    if not text:
        return fallback
    replacement_count = text.count("\ufffd")
    high_bit_count = sum(1 for ch in text if ord(ch) >= 0x80)
    if replacement_count > 0:
        return fallback
    if high_bit_count >= 4 and not any("\u4e00" <= ch <= "\u9fff" for ch in text):
        return fallback
    return text


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) < 4:
        return str(family_id).replace("/", "-")
    return "-".join(parts[2:])


def _root_relative_prefix(repo_root: Path, output_path: Path) -> str:
    relative_path = Path(_relative(repo_root, output_path))
    depth = len(relative_path.parent.parts)
    return "../" * depth if depth > 0 else "./"


def _manifest_path(repo_root: Path) -> Path:
    return repo_root.joinpath(*PROGRAM_MANIFEST_RELATIVE_PATH)


def has_foundation_dll_audit_manifest(repo_root: Path) -> bool:
    return _manifest_path(repo_root).is_file()


def _load_program_manifest(repo_root: Path) -> dict[str, Any]:
    path = _manifest_path(repo_root)
    if not path.is_file():
        raise RuntimeError(f"foundation dll audit manifest is missing: {_relative(repo_root, path)}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    payload["roadmapPath"] = _normalized(
        _string(payload.get("roadmapPath")) or Path(*ROADMAP_PLAN_RELATIVE_PATH).as_posix()
    )
    payload["designPath"] = _normalized(_string(payload.get("designPath")))
    payload["statusPath"] = _normalized(
        _string(payload.get("statusPath")) or Path(*ROADMAP_STATUS_RELATIVE_PATH).as_posix()
    )
    payload["authorityTaskId"] = _normalized(_string(payload.get("authorityTaskId")))
    payload["authorityPhase"] = _string(payload.get("authorityPhase"))
    payload["scopeSource"] = _string(payload.get("scopeSource"))
    payload["title"] = _safe_display_text(payload.get("title"), "Foundation DLL Translation Audit")
    for template in list(payload.get("projectTemplates") or []):
        template["displayName"] = _safe_display_text(template.get("displayName"), _string(template.get("code")) or "Project")
        fallback_method = f"Formal verification for {template['displayName']}"
        template["verificationMethod"] = _safe_display_text(template.get("verificationMethod"), fallback_method)
    return payload


def _load_capability_ledger(repo_root: Path) -> dict[str, Any] | None:
    path = repo_root.joinpath(*LEDGER_RELATIVE_PATH)
    if not path.is_file():
        return None
    return json.loads(path.read_text(encoding="utf-8"))


def _root_relative_link(root_prefix: str, path_text: str) -> str:
    normalized = _normalized(path_text)
    if not normalized:
        return ""
    return root_prefix + normalized


def _build_ledger_lookup(ledger: dict[str, Any] | None) -> dict[str, dict[str, Any]]:
    if ledger is None:
        return {}
    dlls = list(ledger.get("dlls") or [])
    return {entry["assemblyName"]: entry for entry in dlls}


def _parse_markdown_table_rows(roadmap_text: str) -> dict[str, dict[str, str]]:
    rows: dict[str, dict[str, str]] = {}
    for raw_line in roadmap_text.splitlines():
        line = raw_line.strip()
        if not line.startswith("| `202"):
            continue
        columns = [segment.strip() for segment in line.split("|")[1:-1]]
        if len(columns) < 6:
            continue
        task_id = columns[0].strip("`")
        rows[task_id] = {
            "taskId": task_id,
            "phase": columns[1].strip("`"),
            "status": columns[2].strip("`"),
            "owner": columns[3].strip("`"),
            "purpose": columns[4],
            "dependsOn": columns[5].strip("`"),
        }
    return rows


def _task_status_path(repo_root: Path, task_id: str) -> Path | None:
    candidates = [
        repo_root / "docs" / "dev" / "in-progress" / task_id / "STATUS.md",
        repo_root / "docs" / "dev" / "completed" / task_id / "STATUS.md",
        repo_root / "docs" / "dev" / "hanging" / task_id / "STATUS.md",
        repo_root / "docs" / "dev" / "abandoned" / task_id / "STATUS.md",
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    return None


def _extract_path_references(repo_root: Path, markdown_path: Path | None) -> list[str]:
    if markdown_path is None or not markdown_path.is_file():
        return []
    seen: set[str] = set()
    references: list[str] = []
    for match in ARTIFACT_PATH_PATTERN.finditer(markdown_path.read_text(encoding="utf-8")):
        candidate = _normalized(match.group("path").rstrip(".,)"))
        if candidate in seen:
            continue
        seen.add(candidate)
        resolved = repo_root / candidate
        if resolved.exists():
            references.append(candidate)
    return references


def _classify_artifact_kind(path_text: str, *, is_dir: bool) -> tuple[str, str]:
    normalized = _normalized(path_text).lower()
    if is_dir:
        return "directory", "directory"
    if normalized.endswith(".md"):
        return "report-md", "file"
    if normalized.endswith(".json"):
        return "report-json", "file"
    if normalized.endswith((".cs", ".cpp", ".c", ".h", ".hpp", ".sln", ".csproj", ".props", ".targets")):
        return "code-file", "file"
    if normalized.endswith(".log"):
        return "log", "file"
    return "binary", "file"


def _artifact_role(project_code: str, path_text: str) -> str:
    normalized = _normalized(path_text).lower()
    if project_code == "audit-input-and-ledger":
        return "input"
    if project_code in {"managed-proof", "native-proof", "hotupdate-proof"}:
        return "proof"
    if project_code == "benchmark":
        return "benchmark"
    if project_code == "codegen-review":
        return "review" if normalized.endswith(".md") or "review" in normalized else "codegen"
    if project_code == "completion-certification":
        return "completion"
    return "log"


def _artifact_record(repo_root: Path, assembly_name: str, project_code: str, path_text: str) -> dict[str, Any]:
    resolved = repo_root / Path(path_text)
    exists = resolved.exists()
    is_dir = resolved.is_dir()
    artifact_kind, link_target_type = _classify_artifact_kind(path_text, is_dir=is_dir)
    return {
        "assemblyName": assembly_name,
        "projectCode": project_code,
        "displayName": resolved.name if resolved.name else path_text,
        "path": _normalized(path_text),
        "artifactKind": artifact_kind,
        "linkTargetType": link_target_type,
        "role": _artifact_role(project_code, path_text),
        "required": False,
        "exists": exists,
    }


def _project_artifacts(
    repo_root: Path,
    *,
    assembly_name: str,
    project_template: dict[str, Any],
    evidence_paths: list[str],
    support_paths: list[str],
) -> tuple[list[dict[str, Any]], list[str], bool]:
    keywords = [_normalized(item).lower() for item in _list(project_template.get("artifactKeywords"))]
    matched_evidence: list[str] = []
    for evidence_path in evidence_paths:
        normalized = _normalized(evidence_path).lower()
        if not normalized.startswith("artifacts/"):
            continue
        if any(keyword in normalized for keyword in keywords):
            matched_evidence.append(evidence_path)
    primary_paths = list(dict.fromkeys(matched_evidence))
    support_refs = [path for path in list(dict.fromkeys(support_paths)) if _normalized(path) not in {_normalized(item) for item in primary_paths}]
    artifacts = [
        _artifact_record(
            repo_root,
            assembly_name=assembly_name,
            project_code=_string(project_template.get("code")),
            path_text=path_text,
        )
        for path_text in primary_paths
    ]
    primary_path_set = {_normalized(item) for item in primary_paths}
    has_primary_evidence = any(
        artifact["exists"] and artifact["path"] in primary_path_set
        for artifact in artifacts
    )
    return artifacts, support_refs, has_primary_evidence


def _dll_state(
    assembly_entry: dict[str, Any],
    *,
    roadmap_rows: dict[str, dict[str, str]],
    active_task_ids: set[str],
) -> tuple[str, str, str]:
    roadmap_task_id = _string(assembly_entry.get("roadmapTaskId"))
    roadmap_row = dict(roadmap_rows.get(roadmap_task_id) or {})
    roadmap_status = _string(roadmap_row.get("status")) or "planned"
    dll_state = ROADMAP_STATUS_TO_DLL_STATE.get(roadmap_status, "not-started")
    for task_id in [_string(item) for item in _list(assembly_entry.get("followupTaskIds")) if _string(item)]:
        if task_id in active_task_ids:
            return dll_state, task_id, _string((roadmap_rows.get(task_id) or {}).get("purpose"))
    return dll_state, roadmap_task_id, _string(roadmap_row.get("purpose"))


def _compute_capability_closure(families: list[dict[str, Any]]) -> dict[str, Any]:
    total = len(families)
    closed = sum(1 for f in families if f.get("closureStatus") == "closed")
    waived = sum(1 for f in families if f.get("closureStatus") == "waived")
    excluded = sum(1 for f in families if f.get("closureStatus") == "excluded")
    platform_blocked = sum(1 for f in families if f.get("closureStatus") == "platform-blocked")
    in_progress = sum(1 for f in families if f.get("closureStatus") == "in-progress")
    closure_pct = round((closed / total) * 100, 2) if total > 0 else 0.0
    return {
        "totalFamilies": total,
        "closedFamilies": closed,
        "waivedFamilies": waived,
        "excludedFamilies": excluded,
        "platformBlockedFamilies": platform_blocked,
        "inProgressFamilies": in_progress,
        "closurePercent": closure_pct,
    }


def _compute_gate_progress(
    families: list[dict[str, Any]],
    *,
    project_templates: list[dict[str, Any]],
) -> dict[str, Any]:
    gate_codes = [tmpl.get("code") for tmpl in project_templates if tmpl.get("code") != "completion-certification"]
    all_required = 0
    all_passed = 0
    for family in families:
        gates = dict(family.get("verificationGates") or {})
        for code in gate_codes:
            state = _string(gates.get(code))
            if state and state != "not-required":
                all_required += 1
                if state == "passed":
                    all_passed += 1
    progress_pct = round((all_passed / all_required) * 100, 2) if all_required > 0 else 0.0
    return {
        "totalRequiredGates": all_required,
        "passedGates": all_passed,
        "progressPercent": progress_pct,
    }


def _compute_waiver_summary(families: list[dict[str, Any]]) -> dict[str, Any]:
    total = 0
    active = 0
    expired = 0
    exclusions = 0
    platform_blocked = 0
    for family in families:
        for record in list(family.get("waiverRecords") or []):
            total += 1
            rtype = _string(record.get("type"))
            if rtype == "exclusion":
                exclusions += 1
            elif rtype == "platform-blocked":
                platform_blocked += 1
            else:
                if record.get("status") == "expired":
                    expired += 1
                else:
                    active += 1
    return {
        "totalWaivers": total,
        "activeWaivers": active,
        "expiredWaivers": expired,
        "totalExclusions": exclusions,
        "totalPlatformBlocked": platform_blocked,
    }


_EXECUTION_TO_GATE_STATE: dict[str, str] = {
    "passed": "passed",
    "blocked": "in-progress",
    "in-progress": "in-progress",
    "pending": "pending",
    "missing-evidence": "in-progress",
    "not-required": "not-required",
}


def _auto_derive_family(
    assembly_name: str,
    projects: list[dict[str, Any]],
    project_templates: list[dict[str, Any]],
) -> dict[str, Any]:
    verification_gates: dict[str, str] = {}
    for project in projects:
        code = _string(project.get("projectCode"))
        if code == "completion-certification":
            continue
        policy = _string(project.get("policyState"))
        if policy == "not-required":
            continue
        ex_state = _string(project.get("executionState"))
        gate_state = _EXECUTION_TO_GATE_STATE.get(ex_state, "pending")
        if gate_state != "not-required":
            verification_gates[code] = gate_state
    return {
        "familyId": f"family/{assembly_name}/all",
        "displayName": f"{assembly_name} Full Surface",
        "description": f"Auto-derived capability family for {assembly_name}",
        "denominatorStatus": "auto-derived",
        "closureStatus": "in-progress",
        "verificationGates": verification_gates,
        "methodCount": 0,
        "implementationFamilies": [],
        "sourceGroups": [],
    }


def _validate_ledger_dll(entry: dict[str, Any]) -> list[str]:
    warnings: list[str] = []
    families = list(entry.get("families") or [])
    seen_ids: set[str] = set()
    valid_states = {"closed", "waived", "excluded", "platform-blocked", "in-progress"}
    valid_denominator_states = {"candidate-derived", "audit-confirmed", "auto-derived"}
    valid_gate_states = {"pending", "in-progress", "passed", "failed", "blocked", "missing-evidence", "not-required"}
    dll_denominator_status = _string(entry.get("denominatorStatus"))
    if dll_denominator_status and dll_denominator_status not in valid_denominator_states:
        warnings.append(f"Invalid denominatorStatus '{dll_denominator_status}' for DLL {entry.get('assemblyName')}")
    for family in families:
        fid = _string(family.get("familyId"))
        if fid in seen_ids:
            warnings.append(f"Duplicate familyId: {fid}")
        seen_ids.add(fid)
        status = _string(family.get("closureStatus"))
        if status and status not in valid_states:
            warnings.append(f"Invalid closureStatus '{status}' for {fid}")
        family_denominator_status = _string(family.get("denominatorStatus"))
        if family_denominator_status and family_denominator_status not in valid_denominator_states:
            warnings.append(f"Invalid denominatorStatus '{family_denominator_status}' for {fid}")
        for gate_code, gate_state in dict(family.get("verificationGates") or {}).items():
            if gate_state not in valid_gate_states:
                warnings.append(f"Invalid gate state '{gate_state}' for {fid}/{gate_code}")
    return warnings


def _derive_dll_denominator_status(ledger_entry: dict[str, Any] | None, families: list[dict[str, Any]], family_source: str) -> str:
    if family_source == "auto-derived":
        return "auto-derived"
    explicit = _string((ledger_entry or {}).get("denominatorStatus"))
    if explicit:
        return explicit
    family_states = {_string(family.get("denominatorStatus")) for family in families if _string(family.get("denominatorStatus"))}
    if family_states == {"audit-confirmed"}:
        return "audit-confirmed"
    return "candidate-derived"


def _render_progress_bar(label: str, numerator: int, denominator: int, pct: float, *, css_class: str) -> str:
    fill_width = min(pct, 100.0)
    return (
        f'<div class="progress-bar-container">'
        f'<div class="progress-label"><span>{escape(label)}</span><span>{numerator}/{denominator} ({pct:.1f}%)</span></div>'
        f'<div class="progress-bar"><div class="progress-bar-fill {escape(css_class)}" style="width: {fill_width:.1f}%"></div></div>'
        f'</div>'
    )


def _render_mini_bar(pct: float, numerator: int, denominator: int) -> str:
    if denominator == 0:
        return '<span class="mini-bar"><span class="mini-fill" style="width:0%"></span></span> <span class="mini-label">0/0</span>'
    fill_width = min(pct, 100.0)
    css = "high" if pct >= 100 else ("medium" if pct >= 50 else "low")
    return (
        f'<span class="mini-bar" title="{numerator}/{denominator}">'
        f'<span class="mini-fill {escape(css)}" style="width: {fill_width:.1f}%"></span>'
        f'</span>'
        f' <span class="mini-label">{numerator}/{denominator}</span>'
    )


def _collect_support_refs(
    repo_root: Path,
    *,
    program_manifest: dict[str, Any],
    task_status_paths: list[Path],
) -> list[str]:
    paths: list[str] = []
    for key in ("roadmapPath", "designPath", "statusPath", "subjectEntry"):
        text = _string(program_manifest.get(key))
        if text and (repo_root / text).exists():
            paths.append(_normalized(text))
    paths.extend(_relative(repo_root, path) for path in task_status_paths if path is not None)
    return list(dict.fromkeys(path for path in paths if path))


def _evidence_task_ids(assembly_entry: dict[str, Any]) -> list[str]:
    explicit = [_string(item) for item in _list(assembly_entry.get("evidenceTaskIds")) if _string(item)]
    if explicit:
        return explicit
    return [
        _string(assembly_entry.get("roadmapTaskId")),
        *[_string(item) for item in _list(assembly_entry.get("followupTaskIds")) if _string(item)],
    ]


def _display_phase(program_manifest: dict[str, Any], assembly_entry: dict[str, Any]) -> str:
    return (
        _string(assembly_entry.get("displayPhase"))
        or _string(program_manifest.get("authorityPhase"))
        or _string(assembly_entry.get("phase"))
    )


def _display_roadmap_task_id(program_manifest: dict[str, Any], assembly_entry: dict[str, Any]) -> str:
    return (
        _string(assembly_entry.get("displayRoadmapTaskId"))
        or _string(program_manifest.get("authorityTaskId"))
        or _string(assembly_entry.get("roadmapTaskId"))
    )


def _ordered_assembly_entries(
    program_manifest: dict[str, Any],
    *,
    ledger: dict[str, Any] | None,
) -> list[dict[str, Any]]:
    manifest_entries = sorted(
        [dict(item) for item in list(program_manifest.get("assemblies") or [])],
        key=lambda item: int(item.get("orderIndex") or 0),
    )
    if _string(program_manifest.get("scopeSource")) != "capability-family-ledger" or ledger is None:
        return manifest_entries

    manifest_by_name = {
        _string(item.get("assemblyName")): dict(item)
        for item in manifest_entries
        if _string(item.get("assemblyName"))
    }
    ordered_entries: list[dict[str, Any]] = []
    for index, ledger_entry in enumerate(list(ledger.get("dlls") or []), start=1):
        assembly_name = _string(ledger_entry.get("assemblyName"))
        if not assembly_name:
            continue
        merged = dict(manifest_by_name.get(assembly_name) or {})
        merged["assemblyName"] = assembly_name
        merged["orderIndex"] = int(merged.get("orderIndex") or index)
        ordered_entries.append(merged)
    return ordered_entries


def _build_projects(
    repo_root: Path,
    *,
    assembly_entry: dict[str, Any],
    program_manifest: dict[str, Any],
    roadmap_rows: dict[str, dict[str, str]],
    active_task_ids: set[str],
) -> tuple[list[dict[str, Any]], str]:
    project_templates = list(program_manifest.get("projectTemplates") or [])
    assembly_name = _string(assembly_entry.get("assemblyName"))
    task_ids = _evidence_task_ids(assembly_entry)
    task_status_paths = [path for path in (_task_status_path(repo_root, task_id) for task_id in task_ids) if path is not None]
    evidence_paths: list[str] = []
    for status_path in task_status_paths:
        evidence_paths.extend(_extract_path_references(repo_root, status_path))
    evidence_paths = list(dict.fromkeys(evidence_paths))
    support_paths = _collect_support_refs(
        repo_root,
        program_manifest=program_manifest,
        task_status_paths=task_status_paths,
    )
    dll_state, active_task_id, blocker_summary = _dll_state(
        assembly_entry,
        roadmap_rows=roadmap_rows,
        active_task_ids=active_task_ids,
    )
    override = ROADMAP_PHASE_STATUS_OVERRIDES.get(assembly_name)
    if override is not None:
        dll_state = _string(override.get("dllState")) or dll_state
        blocker_summary = _string(override.get("blockingReason")) or blocker_summary
    policies = dict(assembly_entry.get("projectPolicies") or {})
    project_rows: list[dict[str, Any]] = []
    unresolved_codes: list[str] = []
    evidence_map: dict[str, bool] = {}
    for template in project_templates:
        code = _string(template.get("code"))
        policy_state = _string(policies.get(code) or template.get("defaultPolicyState") or "required")
        artifacts, support_refs, has_primary_evidence = _project_artifacts(
            repo_root,
            assembly_name=assembly_name,
            project_template=template,
            evidence_paths=evidence_paths,
            support_paths=support_paths,
        )
        evidence_map[code] = has_primary_evidence
        if code == "completion-certification":
            execution_state = "pending"
        elif policy_state == "not-required":
            execution_state = "not-required"
        elif has_primary_evidence:
            execution_state = "passed"
        elif policy_state == "conditional":
            execution_state = "pending" if dll_state != "completed" else "not-required"
        else:
            execution_state = "pending"
            unresolved_codes.append(code)
        if (
            code != "completion-certification"
            and policy_state not in {"not-required", "conditional"}
            and execution_state not in {"passed", "not-required"}
            and code not in unresolved_codes
        ):
            unresolved_codes.append(code)
        project_rows.append(
            {
                "projectCode": code,
                "displayName": _string(template.get("displayName")),
                "policyState": policy_state,
                "executionState": execution_state,
                "verificationMethod": _string(template.get("verificationMethod")),
                "verificationTarget": assembly_name,
                "completionRule": _string(template.get("completionRule") or ""),
                "artifacts": artifacts,
                "supportRefs": support_refs,
                "blockers": [],
                "updatedAt": "",
            }
        )
    ordered_unresolved_codes = [
        code
        for code in PROJECT_PROGRESS_ORDER
        if code in unresolved_codes and code != "completion-certification"
    ]
    unresolved_code = ordered_unresolved_codes[0] if ordered_unresolved_codes else ""
    completion_state = "pending"
    if not unresolved_code:
        completion_state = "passed" if evidence_map.get("completion-certification") else "missing-evidence"
    elif dll_state == "completed":
        completion_state = "missing-evidence"
    elif dll_state in {"blocked", "in-progress"}:
        completion_state = dll_state

    for project_row in project_rows:
        if project_row["projectCode"] == unresolved_code and dll_state in {"blocked", "in-progress", "completed"}:
            project_row["executionState"] = "missing-evidence" if dll_state == "completed" else dll_state
            if blocker_summary:
                project_row["blockers"] = [
                    {
                        "code": _string(active_task_id or assembly_entry.get("roadmapTaskId")),
                        "message": blocker_summary,
                    }
                ]
            break

    for project_row in project_rows:
        if project_row["projectCode"] == "completion-certification":
            project_row["executionState"] = completion_state
            if completion_state in {"blocked", "in-progress", "missing-evidence"} and blocker_summary:
                project_row["blockers"] = [
                    {
                        "code": _string(active_task_id or assembly_entry.get("roadmapTaskId")),
                        "message": blocker_summary,
                    }
                ]
            break

    if dll_state in {"blocked", "in-progress", "completed"} and unresolved_code:
        return project_rows, "completion-certification"

    current_project = next(
        (
            row["projectCode"]
            for row in project_rows
            if row["policyState"] != "not-required" and row["executionState"] not in {"passed", "not-required"}
        ),
        "completion-certification",
    )
    return project_rows, current_project


def _roadmap_rows(repo_root: Path, roadmap_path_text: str) -> dict[str, dict[str, str]]:
    roadmap_path = repo_root / Path(roadmap_path_text)
    if not roadmap_path.is_file():
        return {}
    return _parse_markdown_table_rows(roadmap_path.read_text(encoding="utf-8"))


def _active_task_ids(repo_root: Path) -> set[str]:
    active_path = repo_root / "docs" / "dev" / "ACTIVE.md"
    if not active_path.is_file():
        return set()
    for line in active_path.read_text(encoding="utf-8").splitlines():
        if line.startswith("task_id:"):
            task_id = line.split(":", 1)[1].strip()
            return {task_id} if task_id else set()
    return set()


def build_foundation_dll_audit_payload(repo_root: Path) -> dict[str, Any]:
    program_manifest = _load_program_manifest(repo_root)
    roadmap_rows = _roadmap_rows(repo_root, _string(program_manifest.get("roadmapPath")))
    active_task_ids = _active_task_ids(repo_root)
    ledger = _load_capability_ledger(repo_root)
    ledger_lookup = _build_ledger_lookup(ledger)
    has_ledger = ledger is not None
    project_templates = list(program_manifest.get("projectTemplates") or [])
    dll_rows: list[dict[str, Any]] = []
    artifact_rows: list[dict[str, Any]] = []
    for assembly_entry in _ordered_assembly_entries(program_manifest, ledger=ledger):
        projects, current_project = _build_projects(
            repo_root,
            assembly_entry=assembly_entry,
            program_manifest=program_manifest,
            roadmap_rows=roadmap_rows,
            active_task_ids=active_task_ids,
        )
        dll_state, _, blocker_summary = _dll_state(
            assembly_entry,
            roadmap_rows=roadmap_rows,
            active_task_ids=active_task_ids,
        )
        assembly_name = _string(assembly_entry.get("assemblyName"))
        dll_record = {
            "assemblyName": assembly_name,
            "orderIndex": int(assembly_entry.get("orderIndex") or 0),
            "phase": _display_phase(program_manifest, assembly_entry),
            "roadmapTaskId": _display_roadmap_task_id(program_manifest, assembly_entry),
            "dllState": dll_state,
            "currentProject": current_project,
            "blockingReason": blocker_summary,
            "riskTags": list(assembly_entry.get("riskTags") or []),
            "projects": projects,
        }
        override = ROADMAP_PHASE_STATUS_OVERRIDES.get(assembly_name)
        if override is not None:
            dll_record["phase"] = _string(override.get("phase")) or dll_record["phase"]
            dll_record["dllState"] = _string(override.get("dllState")) or dll_record["dllState"]
            dll_record["currentProject"] = _string(override.get("currentProject")) or dll_record["currentProject"]
            dll_record["blockingReason"] = _string(override.get("blockingReason")) or dll_record["blockingReason"]
        if has_ledger:
            ledger_entry = ledger_lookup.get(assembly_name)
            families = list((ledger_entry or {}).get("families") or [])
            family_source = ""
            if not families:
                families = [_auto_derive_family(assembly_name, projects, project_templates)]
                dll_record["familySource"] = "auto-derived"
                family_source = "auto-derived"
            family_claims_snapshot = family_verification_claims_module.build_family_verification_claims_snapshot(
                repo_root,
                assembly_name=assembly_name,
                owner_subject_id="SolutionCorePack",
                families=families,
                projects=projects,
            )
            native_proof_claims = {
                _string(item.get("claimId")): dict(item)
                for item in list(family_claims_snapshot.get("claims") or [])
            }
            family_snapshot = verification_kernel_module.build_family_verification_snapshot(
                repo_root,
                assembly_name=assembly_name,
                owner_subject_id="SolutionCorePack",
                families=families,
                native_proof_claims=native_proof_claims,
                projects=projects,
            )
            truth_contracts_snapshot = truth_contracts_module.load_truth_contracts_snapshot(
                repo_root,
                assembly_name=assembly_name,
                owner_subject_id="SolutionCorePack",
                families=families,
            )
            families = list(family_snapshot.get("families") or [])
            for family_record in families:
                family_id = _string(family_record.get("familyId"))
                case_indexes = case_index_loader_module.load_family_case_indexes(
                    repo_root,
                    assembly_name=assembly_name,
                    family_id=family_id,
                )
                review_bundle = case_index_loader_module.load_family_review_bundle(
                    repo_root,
                    assembly_name=assembly_name,
                    family_id=family_id,
                )
                native_proof = dict(family_record.get("nativeProof") or {})
                native_proof["testCases"] = case_indexes["testCases"]
                native_proof["benchmarkCases"] = case_indexes["benchmarkCases"]
                native_proof["hotupdateCases"] = case_indexes["hotupdateCases"]
                family_record["nativeProof"] = native_proof
                # Inject real case counts into managed/hotupdate/benchmark proof objects
                test_case_count = len(case_indexes["testCases"])
                hotupdate_case_count = len(case_indexes["hotupdateCases"])
                benchmark_case_count = len(case_indexes["benchmarkCases"])
                managed_case_items = [
                    {"memberName": _string(item.get("memberName")), "detail": _string(item.get("routeCode") or "n/a")}
                    for item in case_indexes["testCases"]
                ]
                hotupdate_case_items = [
                    {"memberName": _string(item.get("memberName")), "detail": f'direction={_string(item.get("direction") or "n/a")}'}
                    for item in case_indexes["hotupdateCases"]
                ]
                benchmark_case_items = [
                    {"memberName": _string(item.get("memberName")), "detail": f'profile={_string(item.get("profileCode") or "n/a")}'}
                    for item in case_indexes["benchmarkCases"]
                ]
                for proof_key, case_count, items, section_label in (
                    ("managedProof", test_case_count or int(family_record.get("methodCount") or 0), managed_case_items, "Tests"),
                    ("hotupdateProof", hotupdate_case_count or 1, hotupdate_case_items, "HotUpdate"),
                    ("benchmarkProof", benchmark_case_count or 1, benchmark_case_items, "Benchmarks"),
                ):
                    proof = dict(family_record.get(proof_key) or {})
                    proof["denominator"] = case_count
                    current_num = int(proof.get("numerator") or 0)
                    proof["numerator"] = min(current_num, case_count) if current_num > 0 else 0
                    proof["progressPercent"] = round((proof["numerator"] / proof["denominator"]) * 100, 2) if proof["denominator"] > 0 else 0.0
                    proof["caseItems"] = items
                    proof["caseSectionLabel"] = section_label
                    # Inject benchmark comparison report into benchmarkProof
                    if proof_key == "benchmarkProof":
                        comparison_path = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id) / "benchmark-comparison-report.json"
                        if comparison_path.is_file():
                            try:
                                cmp_data = json.loads(comparison_path.read_text(encoding="utf-8"))
                                summary_data = dict(cmp_data.get("summary") or {})
                                proof["averageSpeedupPercent"] = summary_data.get("averageSpeedupPercent", 0.0)
                                proof["nativeFasterCount"] = summary_data.get("nativeFasterCount", 0)
                                proof["managedFasterCount"] = summary_data.get("managedFasterCount", 0)
                                proof["comparisonMethodResults"] = list(cmp_data.get("methodResults") or [])
                            except (json.JSONDecodeError, OSError):
                                pass
                    # Inject hotupdate verification report into hotupdateProof
                    if proof_key == "hotupdateProof":
                        report_path = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id) / "hotupdate-verification-report.json"
                        if report_path.is_file():
                            try:
                                report_data = json.loads(report_path.read_text(encoding="utf-8"))
                                summary_data = dict(report_data.get("summary") or {})
                                proof["passedMethodCount"] = summary_data.get("passedMethods", 0)
                                proof["failedMethodCount"] = summary_data.get("failedMethods", 0)
                                proof["unmatchedMethodCount"] = summary_data.get("unmatchedMethods", 0)
                                proof["verificationMethodResults"] = list(report_data.get("methodResults") or [])
                                # Update numerator/denominator based on actual results
                                total = summary_data.get("totalMethods", 0)
                                if total > 0:
                                    proof["denominator"] = total
                                    proof["numerator"] = summary_data.get("passedMethods", 0)
                                    proof["progressPercent"] = (proof["numerator"] / total) * 100.0
                            except (json.JSONDecodeError, OSError):
                                pass
                    family_record[proof_key] = proof
                # Synthetic proof objects for audit-input and codegen-review using methodCount
                mc = int(family_record.get("methodCount") or 0)
                gates = dict(family_record.get("verificationGates") or {})
                for synth_key, synth_gate, synth_label in (
                    ("auditInputProof", "audit-input-and-ledger", "Audit Input"),
                    ("codegenReviewProof", "codegen-review", "CodeGen Review"),
                ):
                    gate_status = _string(gates.get(synth_gate))
                    family_record[synth_key] = {
                        "status": gate_status,
                        "denominator": mc,
                        "numerator": mc if gate_status == "passed" else 0,
                        "progressPercent": 100.0 if gate_status == "passed" and mc > 0 else 0.0,
                        "evidence": [],
                        "caseItems": [],
                        "caseSectionLabel": "",
                    }
                family_record["reviewBundle"] = review_bundle
            dll_record["capabilityClosure"] = _compute_capability_closure(families)
            dll_record["workflowProgress"] = _compute_gate_progress(families, project_templates=project_templates)
            dll_record["capabilityFamilies"] = families
            dll_record["familyVerificationClaims"] = family_claims_snapshot
            dll_record["familyVerificationSnapshot"] = family_snapshot
            dll_record["truthContracts"] = truth_contracts_snapshot
            dll_record["sourceLinks"] = dict((ledger_entry or {}).get("sourceLinks") or {})
            dll_record["waiverSummary"] = _compute_waiver_summary(families)
            dll_record["denominatorStatus"] = _derive_dll_denominator_status(ledger_entry, families, family_source)
            dll_record["schemaVersion"] = 2
        dll_rows.append(dll_record)
        for project in projects:
            for artifact in list(project.get("artifacts") or []):
                artifact_rows.append(dict(artifact))
    dll_rows.sort(key=lambda item: int(item.get("orderIndex") or 0))
    completed_count = sum(1 for item in dll_rows if item["dllState"] == "completed")
    in_progress_rows = [item for item in dll_rows if item["dllState"] == "in-progress"]
    blocked_rows = [item for item in dll_rows if item["dllState"] == "blocked"]
    active_assembly = (
        in_progress_rows[0]["assemblyName"]
        if in_progress_rows
        else blocked_rows[0]["assemblyName"] if blocked_rows else ""
    )
    program_payload = {
        "schemaVersion": 1,
        "programId": _string(program_manifest.get("programId")),
        "title": _string(program_manifest.get("title")),
        "roadmapPath": _string(program_manifest.get("roadmapPath")),
        "designPath": _string(program_manifest.get("designPath")),
        "statusPath": _string(program_manifest.get("statusPath")),
        "subjectEntry": _string(program_manifest.get("subjectEntry")),
        "summary": {
            "dllCount": len(dll_rows),
            "completedCount": completed_count,
            "inProgressCount": len(in_progress_rows),
            "blockedCount": len(blocked_rows),
            "notStartedCount": sum(1 for item in dll_rows if item["dllState"] == "not-started"),
            "progressPercent": round((completed_count / len(dll_rows)) * 100, 2) if dll_rows else 0.0,
            "activeAssembly": active_assembly,
        },
        "scopeAssemblies": [item["assemblyName"] for item in dll_rows],
    }
    if has_ledger:
        all_families: list[dict[str, Any]] = []
        for dll_row in dll_rows:
            all_families.extend(list(dll_row.get("capabilityFamilies") or []))
        program_payload["schemaVersion"] = 2
        program_payload["summary"]["capabilityClosure"] = _compute_capability_closure(all_families)
        total_required = sum(dll_row.get("workflowProgress", {}).get("totalRequiredGates", 0) for dll_row in dll_rows)
        total_passed = sum(dll_row.get("workflowProgress", {}).get("passedGates", 0) for dll_row in dll_rows)
        program_payload["summary"]["workflowProgress"] = {
            "totalRequiredGates": total_required,
            "passedGates": total_passed,
            "progressPercent": round((total_passed / total_required) * 100, 2) if total_required > 0 else 0.0,
        }
        program_payload["summary"]["dllCompletion"] = {
            "completedDllCount": completed_count,
            "blockedDllCount": len(blocked_rows),
            "inProgressDllCount": len(in_progress_rows),
            "notStartedDllCount": sum(1 for item in dll_rows if item["dllState"] == "not-started"),
        }
        authority_snapshot = dict(ledger.get("authoritySnapshotOf") or {}) if isinstance(ledger.get("authoritySnapshotOf"), dict) else {}
        program_payload["authoritySnapshot"] = {
            "snapshotId": _string(ledger.get("snapshotId") or f"snap-{ledger.get('snapshotAt', 'unknown')}"),
            "snapshotAt": _string(ledger.get("snapshotAt", "")),
            "ledgerVersion": _string(ledger.get("schemaVersion", "")),
            "denominatorStrategy": _string(ledger.get("denominatorStrategy", "")),
            "denominatorStatus": _string(ledger.get("denominatorStatus", "")),
            "originalAuthorityDigest": dict(ledger.get("originalAuthorityDigest") or {}),
        }
    matrix_rows = []
    for dll_row in dll_rows:
        matrix_row = {
            "assemblyName": dll_row["assemblyName"],
            "orderIndex": dll_row["orderIndex"],
            "dllState": dll_row["dllState"],
            "currentProject": dll_row["currentProject"],
            "riskTags": dll_row["riskTags"],
        }
        for project in dll_row["projects"]:
            matrix_row[project["projectCode"]] = project["executionState"]
        if has_ledger:
            matrix_row["denominatorStatus"] = dll_row.get("denominatorStatus", "")
            matrix_row["capabilityClosure"] = dll_row.get("capabilityClosure", {
                "totalFamilies": 0, "closedFamilies": 0, "waivedFamilies": 0, "excludedFamilies": 0,
                "platformBlockedFamilies": 0, "inProgressFamilies": 0, "closurePercent": 0.0,
            })
            matrix_row["workflowProgress"] = dll_row.get("workflowProgress", {
                "totalRequiredGates": 0, "passedGates": 0, "progressPercent": 0.0,
            })
        matrix_rows.append(matrix_row)
    return {
        "program": program_payload,
        "dlls": dll_rows,
        "familyVerification": {
            "schemaVersion": 1,
            "assemblies": [
                dict(row.get("familyVerificationSnapshot") or {
                    "schemaVersion": 1,
                    "assemblyName": row.get("assemblyName"),
                    "ownerSubjectId": "SolutionCorePack",
                    "families": list(row.get("capabilityFamilies") or []),
                })
                for row in dll_rows
                if row.get("capabilityFamilies") is not None
            ],
        },
        "familyVerificationClaims": {
            "schemaVersion": 1,
            "assemblies": [
                dict(row.get("familyVerificationClaims") or {
                    "schemaVersion": 1,
                    "assemblyName": row.get("assemblyName"),
                    "ownerSubjectId": "SolutionCorePack",
                    "methodUniverseArtifactPaths": [],
                    "claims": [],
                })
                for row in dll_rows
                if row.get("capabilityFamilies") is not None
            ],
        },
        "truthContracts": {
            "schemaVersion": 1,
            "assemblies": [
                dict(row.get("truthContracts") or {
                    "schemaVersion": 1,
                    "dllCapabilityManifest": {
                        "assemblyName": row.get("assemblyName"),
                        "ownerSubjectId": "SolutionCorePack",
                        "capabilityFamilies": [],
                        "capabilityFamilyCount": 0,
                        "methodUniverseCount": 0,
                    },
                    "capabilityFamilyVerificationContracts": [],
                    "methodCapabilityContracts": [],
                })
                for row in dll_rows
                if row.get("capabilityFamilies") is not None
            ],
        },
        "dllMatrix": {
            "schemaVersion": 2 if has_ledger else 1,
            "rows": matrix_rows,
        },
        "artifactIndex": {
            "schemaVersion": 1,
            "rows": artifact_rows,
        },
    }


def _dashboard_link(path_text: str, *, root_prefix: str) -> str:
    normalized = _normalized(path_text)
    if not normalized:
        return "n/a"
    return f'<a href="{escape(root_prefix + normalized, quote=True)}" target="_blank" rel="noreferrer">{escape(normalized)}</a>'


def _local_link(path_text: str, label: str | None = None) -> str:
    normalized = _normalized(path_text)
    return f'<a href="{escape(normalized, quote=True)}">{escape(label or normalized)}</a>'


def _status_class(value: Any) -> str:
    normalized = re.sub(r"[^a-z0-9]+", "-", _normalized(str(value or "")).lower()).strip("-")
    return normalized or "unknown"


def _status_badge(value: Any) -> str:
    text = _string(value) or "unknown"
    return f'<span class="status status-{_status_class(text)}">{escape(text)}</span>'


def _dll_detail_relative_path(assembly_name: str) -> str:
    return f"dlls/{_string(assembly_name)}.html"


def _dll_json_relative_path(assembly_name: str) -> str:
    return f"dlls/{_string(assembly_name)}.json"


def _dashboard_styles() -> str:
    return """
:root {
  --bg: #f0ebe0;
  --bg-strong: #e0d2b8;
  --panel: #fffcf5;
  --panel-strong: #fff6e0;
  --line: #cdbfa6;
  --ink: #1a1d1b;
  --muted: #4d5a4e;
  --accent: #b5472b;
  --accent-soft: #fce8e0;
  --ok-bg: #c8f0d4;
  --ok-ink: #1a7a3a;
  --warn-bg: #ffdbb5;
  --warn-ink: #b86500;
  --pending-bg: #e0d8cc;
  --pending-ink: #5a4a3a;
  --blocked-bg: #f8c8c0;
  --blocked-ink: #b83020;
}
* { box-sizing: border-box; }
html { scroll-behavior: smooth; }
body {
  margin: 0;
  color: var(--ink);
  background:
    radial-gradient(circle at top left, rgba(152, 85, 58, 0.10), transparent 28%),
    linear-gradient(180deg, #fbf7ef 0%, var(--bg) 58%, #e5ebe2 100%);
  font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
}
a { color: var(--accent); text-decoration: none; }
a:hover { text-decoration: underline; }
main { width: min(1580px, calc(100vw - 32px)); margin: 28px auto 48px; }
.page-header {
  padding: 28px;
  border: 1px solid var(--line);
  background: linear-gradient(135deg, rgba(255, 250, 242, 0.96), rgba(249, 239, 219, 0.92));
  box-shadow: 0 22px 60px rgba(53, 41, 25, 0.10);
}
.eyebrow {
  display: inline-block;
  margin: 0 0 10px;
  padding: 6px 10px;
  border-radius: 999px;
  background: var(--accent-soft);
  color: var(--accent);
  font-size: 12px;
  font-weight: 700;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}
h1 { margin: 0 0 8px; font-size: clamp(30px, 4vw, 54px); letter-spacing: -0.04em; }
h2 { margin: 0 0 14px; font-size: 24px; letter-spacing: -0.02em; }
h3 { margin: 0; font-size: 22px; letter-spacing: -0.03em; }
h4 { margin: 0; font-size: 16px; }
p { margin: 0 0 12px; color: var(--muted); line-height: 1.55; }
.top-nav {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-top: 18px;
}
.top-nav a {
  padding: 9px 12px;
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.72);
}
.summary-grid,
.meta-grid,
.link-grid,
.project-overview-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
  gap: 12px;
}
.summary-grid,
.meta-grid,
.link-grid,
.project-overview-grid,
.section-stack,
.project-grid { margin-top: 14px; }
.summary-card,
.meta-card,
.link-card,
.project-overview,
.dll-card,
.project-card,
section {
  border: 1px solid var(--line);
  background: color-mix(in srgb, var(--panel) 96%, transparent);
  box-shadow: 0 16px 52px rgba(38, 30, 21, 0.08);
}
.summary-card,
.meta-card,
.link-card,
.project-overview,
.project-card { padding: 14px; }
.summary-card strong,
.meta-card strong,
.link-card strong,
.project-overview strong {
  display: block;
  margin-bottom: 8px;
  color: var(--muted);
  font-size: 12px;
  text-transform: uppercase;
  letter-spacing: 0.06em;
}
section { margin-top: 24px; padding: 20px; }
.table-wrap {
  overflow: auto;
  margin-top: 12px;
  border: 1px solid var(--line);
  background: #fffdf9;
}
table { border-collapse: collapse; width: max-content; min-width: 100%; font-size: 13px; }
th, td {
  border-right: 1px solid var(--line);
  border-bottom: 1px solid var(--line);
  padding: 10px 12px;
  text-align: left;
  vertical-align: top;
  max-width: 420px;
  overflow-wrap: anywhere;
}
th {
  position: sticky;
  top: 0;
  background: #e8dbc7;
  z-index: 1;
}
tr:nth-child(even) td { background: #fff7ea; }
.dll-card { padding: 18px; margin-top: 18px; background: linear-gradient(180deg, #fff9ef 0%, #fffdf8 100%); }
.dll-card[id] { scroll-margin-top: 24px; }
.dll-card-header,
.project-card-header {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}
.dll-card-links,
.project-card-links {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}
.pill-link {
  padding: 8px 10px;
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.82);
}
.project-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 12px; }
.project-card { background: #fffdf8; }
.project-summary-line {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 10px 0;
}
.status {
  display: inline-flex;
  align-items: center;
  min-height: 28px;
  padding: 3px 10px;
  border-radius: 999px;
  font-size: 12px;
  font-weight: 700;
  letter-spacing: 0.04em;
  text-transform: uppercase;
}
.status-passed,
.status-completed { background: #b8ebc8; color: #0d6b2a; font-weight: 800; }
.status-blocked { background: var(--blocked-bg); color: var(--blocked-ink); font-weight: 800; }
.status-pending,
.status-conditional,
.status-not-started,
.status-required { background: var(--pending-bg); color: var(--pending-ink); }
.status-in-progress { background: #b8d8f0; color: #1a5a8a; font-weight: 700; }
.status-failed { background: var(--blocked-bg); color: var(--blocked-ink); font-weight: 800; }
.status-warning { background: var(--warn-bg); color: var(--warn-ink); font-weight: 700; }
.list-label {
  margin: 12px 0 6px;
  color: var(--muted);
  font-size: 12px;
  font-weight: 700;
  letter-spacing: 0.06em;
  text-transform: uppercase;
}
ul { margin: 0; padding-left: 18px; }
li + li { margin-top: 6px; }
.back-link {
  display: inline-flex;
  margin-bottom: 16px;
  padding: 8px 12px;
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.78);
}
/* progress axis */
.progress-axis {
  display: flex;
  flex-wrap: wrap;
  gap: 14px;
  margin-top: 18px;
}
.progress-bar-container {
  flex: 1;
  min-width: 200px;
  padding: 14px;
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.72);
}
.progress-label {
  display: flex;
  justify-content: space-between;
  margin-bottom: 8px;
  font-size: 13px;
  font-weight: 600;
}
.progress-bar {
  height: 18px;
  border-radius: 999px;
  background: var(--pending-bg);
  overflow: hidden;
}
.progress-bar-fill {
  height: 100%;
  border-radius: 999px;
  transition: width 0.3s ease;
}
.progress-bar-fill.dll-completion { background: #4a7da5; }
.progress-bar-fill.capability-closure { background: #4a9a5e; }
.progress-bar-fill.workflow-progress { background: #c4843b; }
/* mini bars for tables */
.mini-bar {
  display: inline-block;
  width: 60px;
  height: 10px;
  border-radius: 999px;
  background: var(--pending-bg);
  overflow: hidden;
  vertical-align: middle;
}
.mini-fill {
  display: block;
  height: 100%;
  border-radius: 999px;
}
.mini-fill.high { background: var(--ok-ink); }
.mini-fill.medium { background: #c4843b; }
.mini-fill.low { background: var(--blocked-ink); }
.mini-label {
  margin-left: 6px;
  font-size: 11px;
  color: var(--muted);
  vertical-align: middle;
}
.native-proof-trigger {
  position: relative;
  display: inline-flex;
  align-items: center;
  cursor: default;
}
.native-proof-tooltip {
  position: absolute;
  top: calc(100% + 8px);
  left: 0;
  width: max-content;
  min-width: 0;
  max-width: min(80vw, 1100px);
  padding: 12px;
  border: 1px solid var(--line);
  background: rgba(255, 253, 248, 0.98);
  box-shadow: 0 10px 24px rgba(28, 32, 38, 0.18);
  opacity: 0;
  visibility: hidden;
  pointer-events: none;
  z-index: 30;
  text-align: left;
  white-space: nowrap;
  user-select: text;
  -webkit-user-select: text;
}
.native-proof-trigger:hover .native-proof-tooltip,
.native-proof-trigger:focus-within .native-proof-tooltip,
.native-proof-trigger.is-visible .native-proof-tooltip {
  opacity: 1;
  visibility: visible;
  pointer-events: auto;
}
.native-proof-tooltip-title {
  font-size: 13px;
  font-weight: 700;
  margin-bottom: 8px;
}
.native-proof-tooltip-meta {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 10px;
}
.native-proof-links {
  display: flex;
  flex-direction: column;
  gap: 6px;
}
.native-proof-methods {
  margin: 0 0 10px;
  padding-left: 18px;
  max-height: 220px;
  overflow: auto;
  white-space: nowrap;
}
.native-proof-links a {
  display: block;
  color: var(--link);
  text-decoration: underline;
  white-space: nowrap;
}
/* source links */
.source-links { margin-top: 14px; }
.source-links h3 { font-size: 14px; margin-bottom: 8px; }
.source-links-grid {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}
.source-link {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 6px 10px;
  border: 1px solid var(--line);
  background: rgba(255, 255, 255, 0.82);
  font-size: 12px;
}
.source-link-icon { font-size: 14px; }
.source-link-path {
  color: var(--muted);
  font-family: "Consolas", "Courier New", monospace;
  font-size: 11px;
}
/* waiver */
.waiver-summary {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 10px;
}
.waiver-badge {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  padding: 4px 8px;
  border-radius: 999px;
  font-size: 11px;
  font-weight: 700;
}
.waiver-badge.active { background: var(--warn-bg); color: var(--warn-ink); }
.waiver-badge.exclusion { background: var(--pending-bg); color: var(--pending-ink); }
/* auto-derive notice */
.auto-derive-notice {
  margin-top: 14px;
  padding: 10px 14px;
  border: 1px solid var(--warn-bg);
  background: #fffbf0;
  color: var(--warn-ink);
  font-size: 13px;
  border-radius: 6px;
}
/* family / waiver tables */
.family-table th, .family-table td,
.waiver-table th, .waiver-table td { white-space: nowrap; }
.family-table tr.family-active td:first-child + td { font-weight: 700; }
.benchmark-link {
  display: inline-block;
  padding: 2px 10px;
  border-radius: 4px;
  background: var(--ok-bg);
  color: var(--ok-ink);
  font-weight: 700;
  font-size: 13px;
  white-space: nowrap;
  text-decoration: none;
}
.benchmark-link:hover { background: var(--ok-ink); color: #fff; text-decoration: none; }
/* benchmark detail sections */
.benchmark-detail { margin: 8px 0; }
.benchmark-detail details {
  padding: 12px 16px;
  border: 1px solid var(--line);
  border-radius: 6px;
  background: var(--panel);
}
.benchmark-detail summary {
  cursor: pointer;
  font-size: 14px;
  padding: 4px 0;
}
.benchmark-detail summary:hover { color: var(--accent); }
.provenance-info {
  font-size: 12px;
  color: var(--muted);
  line-height: 1.6;
}
.provenance-info code {
  font-size: 11px;
  background: var(--bg-strong);
  padding: 1px 4px;
  border-radius: 3px;
}
.benchmark-detail-table { width: 100%; border-collapse: collapse; margin-top: 8px; }
.benchmark-detail-table th,
.benchmark-detail-table td {
  padding: 6px 10px;
  border: 1px solid var(--line);
  text-align: right;
  font-size: 13px;
}
.benchmark-detail-table th { background: var(--bg-strong); text-align: right; font-weight: 700; }
.benchmark-detail-table th:first-child,
.benchmark-detail-table td:first-child { text-align: left; }
/* dll detail dual axis */
.dll-axis { margin-top: 0; }
/* tab bar */
.tab-bar {
  display: flex;
  gap: 2px;
  margin-top: 18px;
  border-bottom: 3px solid var(--accent);
}
.tab-button {
  padding: 12px 24px;
  border: none;
  background: #e8e0d4;
  cursor: pointer;
  font-size: 14px;
  font-weight: 700;
  color: var(--muted);
  transition: background 0.2s, color 0.2s;
  margin-bottom: -3px;
  border-radius: 8px 8px 0 0;
  letter-spacing: 0.03em;
}
.tab-button:hover { background: #ddd4c4; color: var(--ink); }
.tab-button.active {
  background: var(--accent);
  color: #fff;
  border-bottom: 3px solid var(--accent);
  box-shadow: 0 -2px 8px rgba(181, 71, 43, 0.15);
}
.tab-content { display: none; }
.tab-content.active { display: block; }
/* heatmap cell colors - more vibrant */
.heatmap-cell-passed { background: #b8ebc8 !important; color: #0d6b2a !important; font-weight: 700 !important; }
.heatmap-cell-in-progress { background: #b8d8f0 !important; color: #1a5a8a !important; font-weight: 700 !important; }
.heatmap-cell-blocked { background: #f8c8c0 !important; color: #b83020 !important; font-weight: 700 !important; }
.heatmap-cell-pending { background: #e0d8cc !important; color: #5a4a3a !important; }
.heatmap-cell-not-required { background: #ece8e0 !important; color: #8a8078 !important; font-style: italic !important; }
.heatmap-cell-missing-evidence { background: #ffdbb5 !important; color: #b86500 !important; font-weight: 700 !important; }
/* expandable rows */
.matrix-row-expandable { cursor: pointer; }
.matrix-row-expandable td:first-child { position: relative; padding-left: 28px; }
.matrix-row-expandable td:first-child::before {
  content: "▶";
  position: absolute;
  left: 8px;
  font-size: 10px;
  color: var(--muted);
  transition: transform 0.2s;
}
.matrix-row-expandable.expanded td:first-child::before { transform: rotate(90deg); }
.matrix-row-detail { display: none; }
.matrix-row-detail.expanded { display: table-row; }
.matrix-row-detail td {
  padding: 14px 20px;
  background: #fffdf7;
}
/* gate detail sections (Layer 3) */
.gate-section {
  margin-top: 18px;
  padding: 18px;
  border: 1px solid var(--line);
  border-left: 4px solid var(--accent);
  background: var(--panel);
}
.gate-section-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 12px;
}
.gate-section-header h3 { margin: 0; font-size: 18px; }
.gate-section-header .status { flex-shrink: 0; }
.gate-section[id] { scroll-margin-top: 20px; }
/* provenance tiering */
.provenance-abbreviated {
  font-size: 11px;
  color: var(--muted);
  line-height: 1.5;
  margin-top: 4px;
}
.provenance-abbreviated code {
  font-size: 10px;
  background: var(--bg-strong);
  padding: 1px 3px;
  border-radius: 2px;
}
.provenance-full {
  font-size: 12px;
  color: var(--muted);
  line-height: 1.7;
  margin-top: 10px;
  padding: 10px;
  border: 1px solid var(--line);
  background: #fffdf6;
}
.provenance-full code {
  font-size: 11px;
  background: var(--bg-strong);
  padding: 1px 4px;
  border-radius: 3px;
}
/* gate anchor link */
.gate-anchor-link {
  color: inherit;
  text-decoration: none;
}
.gate-anchor-link:hover {
  color: var(--accent);
  text-decoration: underline;
}
/* families summary cards */
.family-summary-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 12px;
  margin-top: 14px;
}
.family-summary-card {
  padding: 14px;
  border: 1px solid var(--line);
  background: var(--panel);
}
.family-summary-card strong {
  display: block;
  margin-bottom: 6px;
  color: var(--muted);
  font-size: 12px;
  text-transform: uppercase;
  letter-spacing: 0.06em;
}
/* gate global progress section */
.gate-progress-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
  gap: 12px;
  margin-top: 14px;
}
.gate-progress-card {
  padding: 14px;
  border: 1px solid var(--line);
  background: var(--panel);
}
.gate-progress-card h4 { margin: 0 0 8px; font-size: 14px; }
@media (max-width: 900px) {
  main { width: min(100vw - 18px, 100%); margin: 10px auto 28px; }
  .page-header,
  section,
  .dll-card,
  .summary-card,
  .meta-card,
  .link-card,
  .project-overview,
  .project-card { padding: 14px; }
}
""".strip()


def _project_evidence_count(project: dict[str, Any]) -> int:
    return len(list(project.get("artifacts") or []))


def _project_support_count(project: dict[str, Any]) -> int:
    return len(list(project.get("supportRefs") or []))


def _dll_evidence_count(dll: dict[str, Any]) -> int:
    return sum(_project_evidence_count(project) for project in list(dll.get("projects") or []))


def _dll_support_count(dll: dict[str, Any]) -> int:
    return sum(_project_support_count(project) for project in list(dll.get("projects") or []))


def _render_project_card(project: dict[str, Any], *, root_prefix: str) -> str:
    evidence_items = "".join(
        f"<li>{_dashboard_link(str(artifact.get('path') or ''), root_prefix=root_prefix)} ({escape(str(artifact.get('linkTargetType') or ''))})</li>"
        for artifact in list(project.get("artifacts") or [])
    ) or "<li>n/a</li>"
    support_items = "".join(
        f"<li>{_dashboard_link(str(path), root_prefix=root_prefix)}</li>"
        for path in list(project.get("supportRefs") or [])
    ) or "<li>n/a</li>"
    completion_rule = _string(project.get("completionRule"))
    completion_line = (
        f'<p><strong class="list-label">Completion Rule</strong>{escape(completion_rule)}</p>'
        if completion_rule
        else ""
    )
    return f"""
<div class="project-card">
  <div class="project-card-header">
    <h4>{escape(str(project.get("displayName") or ""))}</h4>
    <div class="project-summary-line">
      {_status_badge(project.get("executionState"))}
      {_status_badge(project.get("policyState"))}
    </div>
  </div>
  <p>{escape(str(project.get("verificationMethod") or ""))}</p>
  {completion_line}
  <div class="project-overview-grid">
    <div class="project-overview"><strong>Project Code</strong>{escape(str(project.get("projectCode") or ""))}</div>
    <div class="project-overview"><strong>Evidence Count</strong>{_project_evidence_count(project)}</div>
    <div class="project-overview"><strong>Support Refs</strong>{_project_support_count(project)}</div>
  </div>
  <div class="list-label">Evidence</div>
  <ul>{evidence_items}</ul>
  <div class="list-label">Support Refs</div>
  <ul>{support_items}</ul>
</div>
""".strip()


def _resolve_source_link(repo_root: Path, root_prefix: str, path_text: str) -> str:
    """Resolve a source link path. If the exact path doesn't exist, try to find the closest existing alternative."""
    normalized = _normalized(path_text)
    if not normalized:
        return ""
    full_path = repo_root / normalized
    if full_path.exists():
        return root_prefix + normalized

    # Try directory variant (remove filename)
    if not normalized.endswith("/"):
        parent_path = repo_root / Path(normalized).parent
        if parent_path.exists():
            return root_prefix + _normalized(str(Path(normalized).parent)) + "/"

    # If this is an artifacts run path, try to find the latest run
    runs_match = re.match(r"(artifacts/subjects/[^/]+/runs/)[^/]+(/.*)?", normalized)
    if runs_match:
        runs_dir = repo_root / runs_match.group(1)
        suffix = runs_match.group(2) or ""
        if runs_dir.is_dir():
            try:
                latest_runs = sorted(
                    [d for d in runs_dir.iterdir() if d.is_dir() and d.name != "last.json"],
                    key=lambda d: d.name,
                    reverse=True,
                )
            except (OSError, NotADirectoryError):
                latest_runs = []
            if latest_runs:
                latest_run = latest_runs[0]
                suffix_stripped = suffix.lstrip("/")
                alt_path = f"{runs_match.group(1)}{latest_run.name}/{suffix_stripped}" if suffix_stripped else f"{runs_match.group(1)}{latest_run.name}/"
                alt_full = repo_root / alt_path
                if alt_full.exists() or alt_full.is_dir():
                    return root_prefix + alt_path
                for subdir in ("analysis/generated/", "analysis/", ""):
                    attempt = f"{runs_match.group(1)}{latest_run.name}/{subdir}"
                    if (repo_root / attempt).exists():
                        return root_prefix + attempt
                return root_prefix + f"{runs_match.group(1)}{latest_run.name}/"

    # Last resort: return the original path anyway
    return root_prefix + normalized


def _render_source_links_block(sl: dict[str, Any], *, repo_root: Path, root_prefix: str) -> str:
    priority = [
        ("subjectSource", "Subject Source", "\U0001F4C1"),
        ("generatedCode", "Generated Code", "\u2699\uFE0F"),
        ("evidence", "Evidence", "\U0001F4CA"),
        ("verificationSource", "Verification Source", "\U0001F4CB"),
        ("authorityDocs", "Authority Docs", "\U0001F4C4"),
    ]
    items: list[str] = []
    for key, label, icon in priority:
        path = _string(sl.get(key))
        if not path:
            continue
        resolved = _resolve_source_link(repo_root, root_prefix, path)
        parts = path.rstrip("/").split("/")
        short_path = "/".join(parts[-3:]) if len(parts) >= 3 else path
        if len(path) > 60:
            short_path = ".../" + short_path
        items.append(
            f'<a href="{escape(resolved, quote=True)}" target="_blank" rel="noreferrer" class="source-link" '
            f'title="{escape(path, quote=True)}">'
            f'<span class="source-link-icon">{icon}</span>'
            f'<span class="source-link-label">{escape(label)}</span>'
            f'<span class="source-link-path">{escape(short_path)}</span>'
            f'</a>'
        )
    if not items:
        return ""
    return f'<div class="source-links"><h3>Source Links</h3><div class="source-links-grid">{"".join(items)}</div></div>'


GATE_LABELS: dict[str, str] = {
    "managed-proof": "Managed Proof",
    "native-proof": "Native Proof",
    "hotupdate-proof": "HotUpdate",
    "benchmark": "Benchmark",
    "test-code": "Test Code",
    "audit-input-and-ledger": "Audit Input",
    "codegen-review": "CodeGen Review",
}
GATE_COLUMNS = tuple(GATE_LABELS.keys())


def _gate_header_tooltip(gate_code: str) -> str:
    tooltips: dict[str, str] = {
        "managed-proof": (
            "Managed Proof: 表示该家族的托管侧测试覆盖情况。"
            "计算方式: 分母 = 该家族的测试用例数 (testCases)，"
            "分子 = 当状态为 passed 时等于分母，否则为 0。"
            "进度 = 分子 / 分母 * 100%。"
            "悬停查看详细测试用例列表"
        ),
        "native-proof": (
            "Native Proof: 表示该家族的原生侧代码生成覆盖情况。"
            "计算方式: 分母 = 该家族中需要进行原生证明的方法总数 (methodSubjectIds)，"
            "分子 = 已通过覆盖率检查的方法数。"
            "进度 = 分子 / 分母 * 100%。"
            "悬停查看每个方法的覆盖状态"
        ),
        "hotupdate-proof": (
            "HotUpdate: 表示该家族的热更新验证覆盖情况。"
            "验证流程: ① Host C# → il2cpp AOT 编译为 C++ 原生代码; "
            "② Patch C# → 编译为 patch.dll (热更包); "
            "③ C++ 运行时通过 LoadHotUpdatePackage + method_replacement::Register 加载并注册热更替换; "
            "④ C++ 验证入口调用原始方法 → 断言热更已生效且结果符合预期。"
            "整个过程在 C++ 侧完成，不依赖 managed 运行时。"
            "计算方式: 分母 = 该家族的热更新测试用例数 (hotupdateCases)，"
            "分子 = 当状态为 passed 时等于分母，否则为 0。"
            "进度 = 分子 / 分母 * 100%。"
            "悬停查看详细热更新用例列表"
        ),
        "benchmark": (
            "Benchmark: 表示该家族的原生代码性能基准测试覆盖情况。"
            "验证流程: ① 通过 dotnet build/run 执行 managed benchmark 生成 baseline 结果; "
            "② 编译 native C++ benchmark 代码通过 benchmark-host 执行计时; "
            "③ 对比脚本按 methodSubjectId 匹配 managed/native 结果; "
            "④ 计算 per-method 提升百分比 (managed_time - native_time) / managed_time * 100%; "
            "⑤ 汇总输出平均提升百分比作为总体指标。"
            "计算方式: 分母 = 该家族的基准测试用例数 (benchmarkCases)，"
            "分子 = 当状态为 passed 时等于分母，否则为 0。"
            "进度 = 分子 / 分母 * 100%。"
            "悬停查看详细基准测试用例列表及 per-method 提升百分比"
        ),
        "test-code": (
            "Test Code: 表示该家族的测试代码存在情况。"
            "状态值: present = 手写测试存在, needs-tests = 缺口, "
            "coverage-widened = 自动生成, no-coverage = 0 方法。"
            "点击跳转查看详情"
        ),
        "audit-input-and-ledger": (
            "Audit Input & Ledger: 表示该家族的审计输入和分类账状态。"
            "评估方式: 检查 audit-input-and-ledger 项目的执行状态。"
            "点击跳转查看详情"
        ),
        "codegen-review": (
            "CodeGen Review: 表示该家族的代码生成审查状态。"
            "评估方式: 检查 codegen-review 项目的执行状态。"
            "点击跳转查看详情"
        ),
    }
    return tooltips.get(gate_code, "")


def _render_benchmark_speedup_cell(
    benchmark_proof: dict[str, Any],
    *,
    assembly_name: str,
    family_id: str,
    root_prefix: str,
    is_detail_page: bool = False,
) -> str:
    """Render benchmark cell: show speedup summary with hyperlink to detail page section.

    The cell displays the average speedup percentage as a clickable link
    that jumps to the family's benchmark section in the DLL detail page.
    """
    avg_speedup = benchmark_proof.get("averageSpeedupPercent")
    if avg_speedup is None:
        return _render_generic_gate_progress_cell(
            {}, gate_proof=benchmark_proof, root_prefix=root_prefix, label='Benchmark'
        )
    slug = _family_slug(family_id)
    anchor = f"benchmark-{slug}"
    detail_href = f"#{anchor}" if is_detail_page else f"./dlls/{assembly_name}.html#{anchor}"
    avg_speedup_fmt = f"{avg_speedup:.1f}"
    native_faster = int(benchmark_proof.get("nativeFasterCount") or 0)
    managed_faster = int(benchmark_proof.get("managedFasterCount") or 0)
    total = native_faster + managed_faster
    if total > 0 and native_faster > managed_faster:
        direction = "faster"
    elif total > 0 and managed_faster >= native_faster:
        direction = "slower"
    else:
        direction = ""
    label = f"{avg_speedup_fmt}% {direction}" if direction else f"{avg_speedup_fmt}%"
    return f'<a href="{escape(detail_href, quote=True)}" class="benchmark-link" title="View benchmark details: {native_faster} native faster, {managed_faster} managed faster">{escape(label)}</a>'


def _render_benchmark_detail_section(family: dict[str, Any], *, assembly_name: str) -> str:
    """Render a collapsible benchmark detail table for a family in the DLL detail page."""
    family_id = _string(family.get("familyId") or "")
    slug = _family_slug(family_id)
    dname = _string(family.get("displayName"))
    benchmark_proof = dict(family.get("benchmarkProof") or {})
    avg_speedup = benchmark_proof.get("averageSpeedupPercent")
    method_results = list(benchmark_proof.get("comparisonMethodResults") or [])
    if not method_results:
        return ""
    avg_speedup_fmt = f"{avg_speedup:.1f}" if avg_speedup is not None else "n/a"
    native_faster = int(benchmark_proof.get("nativeFasterCount") or 0)
    managed_faster = int(benchmark_proof.get("managedFasterCount") or 0)
    matched = sum(1 for r in method_results if r.get("status") == "matched")

    rows = ""
    for r in method_results:
        sid = _short_method_subject_id(_string(r.get("methodSubjectId", "")))
        status = _string(r.get("status", ""))
        managed_ms = r.get("managedElapsedMs")
        native_ms = r.get("nativeElapsedMs")
        speedup = r.get("speedupPercent")
        managed_str = f"{managed_ms:.4f}" if isinstance(managed_ms, (int, float)) else "n/a"
        native_str = f"{native_ms:.4f}" if isinstance(native_ms, (int, float)) else "n/a"
        speedup_str = f"{speedup:.2f}%" if isinstance(speedup, (int, float)) else "n/a"
        status_badge = _status_badge(status)
        rows += f"<tr><td>{escape(sid)}</td><td>{managed_str}</td><td>{native_str}</td><td>{speedup_str}</td><td>{status_badge}</td></tr>"

    return f"""
<div class="benchmark-detail" id="benchmark-{escape(slug, quote=True)}">
  <details>
    <summary><strong>{escape(dname)}</strong> — Average Speedup: {avg_speedup_fmt}% (Native faster: {native_faster}, Managed faster: {managed_faster}, Matched: {matched}/{len(method_results)})</summary>
    <div class="table-wrap">
      <table class="benchmark-detail-table">
        <thead>
          <tr><th>Method</th><th>Managed (ms)</th><th>Native (ms)</th><th>Speedup</th><th>Status</th></tr>
        </thead>
        <tbody>{rows}</tbody>
      </table>
    </div>
  </details>
</div>"""


# Keep original benchmark comparison for tooltip
def _render_benchmark_comparison_section(gate_proof: dict[str, Any]) -> str:
    avg_speedup = gate_proof.get("averageSpeedupPercent")
    if avg_speedup is None:
        return ""
    native_faster = int(gate_proof.get("nativeFasterCount") or 0)
    managed_faster = int(gate_proof.get("managedFasterCount") or 0)
    method_results = list(gate_proof.get("comparisonMethodResults") or [])
    lines = f'<div class="list-label">Benchmark Comparison</div>'
    lines += f'<div>Average Speedup: {avg_speedup}% (Native faster: {native_faster}, Managed faster: {managed_faster})</div>'
    if method_results:
        items = "".join(
            f"<li>{escape(_string(r.get('methodSubjectId', '')))}: speedup={escape(str(r.get('speedupPercent', 'n/a')))}%</li>"
            for r in method_results[:20]
        )
        lines += f'<ul class="native-proof-methods">{items}</ul>'
    return lines


def _family_has_active_gates(gates: dict[str, str]) -> bool:
    """Check if a family has any non-pending, non-not-required gate activity."""
    for k, v in gates.items():
        if k == "completion-certification":
            continue
        v = _string(v)
        if v and v not in ("pending", "not-required", ""):
            return True
    return False


def _render_generic_gate_progress_cell(
    family: dict[str, Any],
    *,
    gate_proof: dict[str, Any],
    root_prefix: str,
    label: str,
) -> str:
    gate_status = _string(gate_proof.get("status"))
    numerator = int(gate_proof.get("numerator") or 0)
    denominator = int(gate_proof.get("denominator") or 0)
    if denominator <= 0 or gate_status in ("not-required", "pending", ""):
        return _status_badge(gate_status)
    progress_bar = _render_mini_bar(
        float(gate_proof.get("progressPercent") or 0.0),
        numerator,
        denominator,
    )
    evidence = list(gate_proof.get("evidence") or [])
    evidence_links = "".join(
        f'<a href="{escape(root_prefix + _normalized(_string(item.get("path"))), quote=True)}" target="_blank" rel="noreferrer">{escape(_string(item.get("label")) or _string(item.get("path")))}</a>'
        for item in evidence[:8]
    ) or "<span class=\"status-muted\">n/a</span>"
    family_name = _string(family.get("displayName")) or label
    # Show case items for the gate (tests for managed-proof, hotupdate cases, benchmarks)
    case_items = list(gate_proof.get("caseItems") or [])
    case_label = _string(gate_proof.get("caseSectionLabel"))
    case_section = ""
    if case_items and case_label:
        items_html = "".join(
            f"<li>{escape(_string(item.get('memberName')))} ({escape(_string(item.get('detail') or 'n/a'))})</li>"
            for item in case_items[:20]
        )
        case_section = f'<div class="list-label">{escape(case_label)}</div><ul class="native-proof-methods">{items_html}</ul>'
    return f"""
<div class="native-proof-trigger" tabindex="0" data-tooltip-delay-ms="500">
  {progress_bar}
  <div class="native-proof-tooltip">
    <div class="native-proof-tooltip-title">{escape(family_name)}</div>
    <div class="native-proof-tooltip-meta">
      {_status_badge(gate_status)}
      <span>{numerator}/{denominator}</span>
    </div>
    {_render_benchmark_comparison_section(gate_proof) if label == 'Benchmark' else ''}
    {case_section}
    <div class="list-label">Evidence</div>
    <div class="native-proof-links">{evidence_links}</div>
    <div class="list-label">Data Provenance</div>
    <div class="provenance-info">
      <div>Source: <code>evaluate_generic_gate()</code> @ <code>verification_kernel.py:204</code></div>
      <div>Gate: <code>{escape(label)}</code></div>
    </div>
  </div>
</div>
""".strip()


def _render_family_table(families: list[dict[str, Any]], *, assembly_name: str = "", root_prefix: str, is_detail_page: bool = False) -> str:
    if not families:
        return ""
    rows = ""
    for idx, family in enumerate(families, start=1):
        dname = _string(family.get("displayName"))
        family_id = _string(family.get("familyId") or "")
        slug = _family_slug(family_id)
        status = _string(family.get("closureStatus"))
        gates = dict(family.get("verificationGates") or {})
        native_proof = dict(family.get("nativeProof") or {})
        native_proof_cell = (
            _render_native_proof_progress_cell(
                family,
                native_proof=native_proof,
                root_prefix=root_prefix,
            )
            if int(native_proof.get("denominator") or 0) > 0
            else _status_badge(native_proof.get("status"))
        )
        managed_proof = dict(family.get("managedProof") or {})
        hotupdate_proof = dict(family.get("hotupdateProof") or {})
        benchmark_proof = dict(family.get("benchmarkProof") or {})
        test_code_proof = dict(family.get("testCodeProof") or {})
        audit_input_proof = dict(family.get("auditInputProof") or {})
        codegen_review_proof = dict(family.get("codegenReviewProof") or {})
        gate_badges = "".join(
            f"<td>{native_proof_cell}</td>" if col == "native-proof"
            else f"<td>{_render_generic_gate_progress_cell(family, gate_proof=managed_proof, root_prefix=root_prefix, label='Managed Proof')}</td>" if col == "managed-proof"
            else f"<td>{_render_generic_gate_progress_cell(family, gate_proof=hotupdate_proof, root_prefix=root_prefix, label='HotUpdate Proof')}</td>" if col == "hotupdate-proof"
            else f"<td>{_render_benchmark_speedup_cell(benchmark_proof, assembly_name=assembly_name, family_id=family_id, root_prefix=root_prefix, is_detail_page=True)}</td>" if col == "benchmark"
            else f"<td>{_render_generic_gate_progress_cell(family, gate_proof=test_code_proof, root_prefix=root_prefix, label='Test Code')}</td>" if col == "test-code"
            else f"<td>{_render_generic_gate_progress_cell(family, gate_proof=audit_input_proof, root_prefix=root_prefix, label='Audit Input')}</td>" if col == "audit-input-and-ledger"
            else f"<td>{_render_generic_gate_progress_cell(family, gate_proof=codegen_review_proof, root_prefix=root_prefix, label='CodeGen Review')}</td>" if col == "codegen-review"
            else f"<td>{_status_badge(gates.get(col, ''))}</td>"
            for col in GATE_COLUMNS
        )
        # Source code links for benchmark
        if assembly_name and slug:
            # Derive benchmark class name from slug: "convert-char" -> "ConvertCharBenchmarks"
            benchmark_class_name = "".join(part.capitalize() for part in slug.split("-")) + "Benchmarks"
            managed_code_href = root_prefix + f"verification/foundation-dll/{assembly_name}/{slug}/benchmark/{benchmark_class_name}.cs"
            native_code_href = root_prefix + f"verification/foundation-dll/{assembly_name}/{slug}/native/BenchmarkNativeEntry.cpp"
            managed_code_cell = f'<td><a href="{escape(managed_code_href, quote=True)}" target="_blank" rel="noreferrer" title="{benchmark_class_name}.cs">.cs</a></td>'
            native_code_cell = f'<td><a href="{escape(native_code_href, quote=True)}" target="_blank" rel="noreferrer" title="BenchmarkNativeEntry.cpp">.cpp</a></td>'
        else:
            managed_code_cell = '<td><span class="status-muted">n/a</span></td>'
            native_code_cell = '<td><span class="status-muted">n/a</span></td>'
        bold_class = ' class="family-active"' if _family_has_active_gates(gates) else ""
        rows += f"<tr{bold_class}><td>{idx}</td><td>{escape(dname)}</td>{gate_badges}{managed_code_cell}{native_code_cell}<td>{_status_badge(status)}</td></tr>"
    gate_headers = "".join(
        f"<th title=\"{_gate_header_tooltip(col)}\">"
        + (f'<a href="#gate-{col}" class="gate-anchor-link">{GATE_LABELS[col]}</a>' if is_detail_page else GATE_LABELS[col])
        + "</th>"
        for col in GATE_COLUMNS
    )
    return f"""
<section>
  <h2>Capability Families</h2>
  <p>Each capability family represents a logical group of methods. A family is considered closed when all its non-exempt verification gates pass.</p>
  <div class="table-wrap">
    <table class="family-table">
      <thead><tr><th>#</th><th>Family</th>{gate_headers}<th title="Managed benchmark source code">Managed Code</th><th title="Native benchmark source code">Native Code</th><th title="Closure status: closed means all non-exempt verification gates pass">closureStatus</th></tr></thead>
      <tbody>{rows}</tbody>
    </table>
  </div>
</section>""".strip()


def _render_waiver_table(families: list[dict[str, Any]]) -> str:
    records: list[dict[str, Any]] = []
    for family in families:
        for r in list(family.get("waiverRecords") or []):
            records.append(r)
    if not records:
        return ""
    rows = ""
    for r in records:
        rows += f"<tr><td>{escape(_string(r.get('type')))}</td><td>{_status_badge(r.get('status'))}</td><td>{escape(_string(r.get('expiresAt')))}</td><td>{escape(_string(r.get('reason')))}</td></tr>"
    return f"""
<section>
  <h2>Waiver Records</h2>
  <div class="table-wrap">
    <table class="waiver-table">
      <thead><tr><th>Type</th><th>Status</th><th>Expires At</th><th>Reason</th></tr></thead>
      <tbody>{rows}</tbody>
    </table>
  </div>
</section>""".strip()


def _render_native_proof_progress_cell(
    family: dict[str, Any],
    *,
    native_proof: dict[str, Any],
    root_prefix: str,
) -> str:
    family_name = _string(family.get("displayName")) or "Native Proof"
    progress_bar = _render_mini_bar(
        float(native_proof.get("progressPercent") or 0.0),
        int(native_proof.get("numerator") or 0),
        int(native_proof.get("denominator") or 0),
    )
    evidence = list(native_proof.get("evidence") or [])
    method_details = list(native_proof.get("methodDetails") or [])
    test_cases = list(native_proof.get("testCases") or [])
    benchmark_cases = list(native_proof.get("benchmarkCases") or [])
    hotupdate_cases = list(native_proof.get("hotupdateCases") or [])
    if not evidence and not method_details:
        return progress_bar
    evidence_links = "".join(
        f'<a href="{escape(root_prefix + _normalized(_string(item.get("path"))), quote=True)}" target="_blank" rel="noreferrer">{escape(_string(item.get("label")) or _string(item.get("path")))}</a>'
        for item in evidence[:4]
    )
    method_items = "".join(
        f"<li>{escape(_short_method_subject_id(_string(item.get('subjectId'))))}</li>"
        for item in method_details
    ) or "<li>n/a</li>"
    test_case_items = "".join(
        f"<li>{escape(_string(item.get('memberName')))} ({escape(_string(item.get('routeCode')) or 'n/a')})</li>"
        for item in test_cases
    ) or "<li>n/a</li>"
    benchmark_case_items = "".join(
        f"<li>{escape(_string(item.get('memberName')))} ({escape(_string(item.get('profileCode')) or 'n/a')})</li>"
        for item in benchmark_cases
    ) or "<li>n/a</li>"
    hotupdate_case_items = "".join(
        f"<li>{escape(_string(item.get('memberName')))} ({escape(_string(item.get('direction')) or 'n/a')})</li>"
        for item in hotupdate_cases
    ) or "<li>n/a</li>"
    return f"""
<div class="native-proof-trigger" tabindex="0" data-tooltip-delay-ms="500">
  {progress_bar}
  <div class="native-proof-tooltip">
    <div class="native-proof-tooltip-title">{escape(family_name)}</div>
    <div class="native-proof-tooltip-meta">
      {_status_badge(native_proof.get("status"))}
      <span>{int(native_proof.get("numerator") or 0)}/{int(native_proof.get("denominator") or 0)}</span>
    </div>
    <div class="list-label">Methods</div>
    <ul class="native-proof-methods">{method_items}</ul>
    <div class="list-label">Tests</div>
    <ul class="native-proof-methods">{test_case_items}</ul>
    <div class="list-label">Benchmarks</div>
    <ul class="native-proof-methods">{benchmark_case_items}</ul>
    <div class="list-label">HotUpdate</div>
    <ul class="native-proof-methods">{hotupdate_case_items}</ul>
    <div class="list-label">Evidence</div>
    <div class="native-proof-links">{evidence_links}</div>
    <div class="list-label">Data Provenance</div>
    <div class="provenance-info">
      <div>Source: <code>evaluate_native_proof()</code> @ <code>verification_kernel.py:106</code></div>
      <div>Evidence: <code>native-reference.runtime-skeleton.coverage.json</code></div>
      <div>Claims: <code>truth-contract:method-capability-contracts</code></div>
    </div>
  </div>
</div>
""".strip()


def _short_method_subject_id(subject_id: str) -> str:
    normalized = _string(subject_id)
    if "/" in normalized:
        normalized = normalized.split("/", 1)[1]
    marker = normalized.find("::")
    if marker < 0:
        return normalized
    colon = normalized.find(":", marker + 2)
    paren = normalized.find("(", colon + 1 if colon >= 0 else marker + 2)
    if colon < 0 or paren < 0:
        return normalized
    return normalized[:colon] + normalized[paren:]


def _dashboard_scripts() -> str:
    return """
<script>
// Tab switching
function switchTab(tabName) {
  var contents = document.querySelectorAll('.tab-content');
  for (var i = 0; i < contents.length; i++) { contents[i].classList.remove('active'); }
  var buttons = document.querySelectorAll('.tab-button');
  for (var i = 0; i < buttons.length; i++) { buttons[i].classList.remove('active'); }
  var target = document.getElementById('tab-' + tabName);
  if (target) { target.classList.add('active'); }
  var btn = document.querySelector('.tab-button[data-tab="' + tabName + '"]');
  if (btn) { btn.classList.add('active'); }
  window.location.hash = tabName;
}
document.addEventListener('DOMContentLoaded', function() {
  var hash = window.location.hash.replace('#', '');
  if (hash) {
    var btn = document.querySelector('.tab-button[data-tab="' + hash + '"]');
    if (btn) switchTab(hash);
  }
});
// Expandable rows
function toggleRow(btn) {
  var row = btn.closest('tr');
  var detail = row.nextElementSibling;
  if (detail && detail.classList.contains('matrix-row-detail')) {
    row.classList.toggle('expanded');
    detail.classList.toggle('expanded');
  }
}
// Native proof tooltips
(function() {
  var triggers = document.querySelectorAll('.native-proof-trigger');
  for (var i = 0; i < triggers.length; i++) {
    var trigger = triggers[i];
    var delayMs = Number(trigger.dataset.tooltipDelayMs || '500');
    var hideTimer = null;
    var clearHide = function() {
      if (hideTimer !== null) { window.clearTimeout(hideTimer); hideTimer = null; }
    };
    var show = function() { clearHide(); trigger.classList.add('is-visible'); };
    var hide = function() {
      clearHide();
      hideTimer = window.setTimeout(function() {
        trigger.classList.remove('is-visible');
        hideTimer = null;
      }, delayMs);
    };
    trigger.addEventListener('pointerenter', show);
    trigger.addEventListener('pointerleave', hide);
    trigger.addEventListener('focusin', show);
    trigger.addEventListener('focusout', function(event) {
      if (trigger.contains(event.relatedTarget)) { return; }
      hide();
    });
  }
})();
</script>
""".strip()


def _render_dll_detail_page(dll: dict[str, Any], *, repo_root: Path, root_prefix: str) -> str:
    projects = list(dll.get("projects") or [])
    project_rows = "".join(
        "<tr>"
        + f"<td>{escape(str(project.get('displayName') or ''))}</td>"
        + f"<td>{_status_badge(project.get('policyState'))}</td>"
        + f"<td>{_status_badge(project.get('executionState'))}</td>"
        + f"<td>{escape(str(project.get('verificationMethod') or ''))}</td>"
        + f"<td>{_project_evidence_count(project)}</td>"
        + f"<td>{_project_support_count(project)}</td>"
        + "</tr>"
        for project in projects
    )
    project_cards = "".join(_render_project_card(project, root_prefix=root_prefix) for project in projects)
    assembly_name = _string(dll.get("assemblyName"))
    families = list(dll.get("capabilityFamilies") or [])
    sl = dict(dll.get("sourceLinks") or {})
    cc = dict(dll.get("capabilityClosure") or {})
    wp = dict(dll.get("workflowProgress") or {})
    family_source = _string(dll.get("familySource"))
    denominator_status = _string(dll.get("denominatorStatus"))

    dual_axis_html = ""
    if cc.get("totalFamilies", 0) > 0:
        dual_axis_html = (
            '<div class="progress-axis dll-axis">'
            + _render_progress_bar("Capability Closure", cc.get("closedFamilies", 0), cc.get("totalFamilies", 0), cc.get("closurePercent", 0.0), css_class="capability-closure")
            + _render_progress_bar("Workflow Progress", wp.get("passedGates", 0), wp.get("totalRequiredGates", 0), wp.get("progressPercent", 0.0), css_class="workflow-progress")
            + '</div>'
        )

    auto_derive_notice = ""
    if family_source == "auto-derived":
        auto_derive_notice = '<div class="auto-derive-notice">&#9432; Capability families for this DLL have been auto-derived from project execution states. Replace with manually curated entries in capability-family-ledger.json.</div>'

    family_section = _render_family_table(families, assembly_name=assembly_name, root_prefix=root_prefix, is_detail_page=True)
    # Benchmark detail tables per family
    benchmark_detail_sections = "".join(
        _render_benchmark_detail_section(family, assembly_name=assembly_name)
        for family in families
    )
    waiver_section = _render_waiver_table(families)
    source_links_html = _render_source_links_block(sl, repo_root=repo_root, root_prefix=root_prefix)

    return f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>{escape(assembly_name)} - Foundation DLL Audit</title>
  <link rel="stylesheet" href="../dashboard.css">
</head>
<body>
  <main>
    <div class="page-header">
      <a class="back-link" href="../dashboard.html">Back To Dashboard</a>
      <div class="eyebrow">DLL Detail</div>
      <h1>{escape(assembly_name)}</h1>
      <p>Inspect current DLL verification status, capability closure, verification projects, and evidence.</p>
      <div class="summary-grid">
        <div class="summary-card"><strong>State</strong>{_status_badge(dll.get("dllState"))}</div>
        <div class="summary-card"><strong>Current Project</strong>{escape(str(dll.get("currentProject") or ""))}</div>
        <div class="summary-card"><strong>Phase</strong>{escape(str(dll.get("phase") or ""))}</div>
        <div class="summary-card"><strong>Roadmap Task</strong>{escape(str(dll.get("roadmapTaskId") or ""))}</div>
        <div class="summary-card"><strong>Evidence Count</strong>{_dll_evidence_count(dll)}</div>
        <div class="summary-card"><strong>Support Refs</strong>{_dll_support_count(dll)}</div>
        <div class="summary-card"><strong>Denominator</strong>{escape(denominator_status or "n/a")}</div>
      </div>
      {dual_axis_html}
      {auto_derive_notice}
      {source_links_html}
      {family_section}
    </div>
    {benchmark_detail_sections if benchmark_detail_sections.strip() else ''}
    {waiver_section if waiver_section.strip() else ''}
    <section>
      <h2>Blocking Context</h2>
      <p>{escape(str(dll.get("blockingReason") or "n/a"))}</p>
    </section>
    <section>
      <h2>Verification Projects</h2>
      <div class="table-wrap">
        <table>
          <thead>
            <tr><th>Project</th><th>Policy</th><th>State</th><th>Verification Method</th><th>Evidence</th><th>Support Refs</th></tr>
          </thead>
          <tbody>{project_rows}</tbody>
        </table>
      </div>
      <div class="project-grid">{project_cards}</div>
    </section>
    <section>
      <h2>Gate Details</h2>
      <p>Detailed verification gate status per family. Each gate section shows numerator/denominator progress, method-level details (where available), evidence links, and complete data provenance information.</p>
      {_render_layer3_gate_sections(families, assembly_name=assembly_name, root_prefix=root_prefix)}
    </section>
  </main>
  {_dashboard_scripts()}
</body>
</html>
"""


def _render_gate_detail_section(
    gate_code: str,
    gate_label: str,
    gate_proof: dict[str, Any],
    *,
    assembly_name: str,
    family_id: str,
    root_prefix: str,
) -> str:
    """Render a Layer 3 gate detail section with full provenance info."""
    status = _string(gate_proof.get("status"))
    numerator = int(gate_proof.get("numerator") or 0)
    denominator = int(gate_proof.get("denominator") or 0)
    progress_pct = float(gate_proof.get("progressPercent") or 0.0)
    evidence = list(gate_proof.get("evidence") or [])
    method_details = list(gate_proof.get("methodDetails") or [])
    case_items = list(gate_proof.get("caseItems") or [])

    evidence_links = "".join(
        f'<a href="{escape(root_prefix + _normalized(_string(item.get("path"))), quote=True)}" target="_blank" rel="noreferrer">{escape(_string(item.get("label")) or _string(item.get("path")))}</a>'
        for item in evidence[:8]
    ) or '<span class="status-muted">n/a</span>'

    method_items = "".join(
        f"<li>{escape(_short_method_subject_id(_string(item.get('subjectId'))))} - {'&#9989;' if item.get('covered') else '&#10060;'}</li>"
        for item in method_details
    ) or ""

    case_items_html = ""
    if case_items:
        case_list = "".join(
            f"<li>{escape(_string(item.get('memberName')))} ({escape(_string(item.get('detail') or 'n/a'))})</li>"
            for item in case_items[:20]
        )
        case_items_html = f'<div class="list-label">Cases</div><ul class="native-proof-methods">{case_list}</ul>'

    denom_display = f"{numerator}/{denominator} ({progress_pct:.1f}%)" if denominator > 0 else "n/a"

    # Provenance info - gate-specific
    provenance_source_map = {
        "native-proof": ("evaluate_native_proof()", "verification_kernel.py:106", "native-reference.runtime-skeleton.coverage.json", "truth-contract:method-capability-contracts"),
        "managed-proof": ("evaluate_generic_gate()", "verification_kernel.py:204", "managed-proof artifacts", "gate-presence:managed-proof"),
        "hotupdate-proof": ("evaluate_generic_gate()", "verification_kernel.py:204", "hotupdate-verification-report.json", "gate-presence:hotupdate-proof"),
        "benchmark": ("evaluate_generic_gate()", "verification_kernel.py:204", "benchmark-comparison-report.json", "gate-presence:benchmark"),
        "test-code": ("evaluate_test_code()", "verification_kernel.py:250", "testCode ledger entry", "ledger:testCode"),
    }
    src_func, src_file, src_evidence, src_claims = provenance_source_map.get(gate_code, ("N/A", "N/A", "N/A", "N/A"))

    return f"""
<section class="gate-section" id="gate-{escape(gate_code, quote=True)}">
  <div class="gate-section-header">
    <h3>{escape(gate_label)}</h3>
    {_status_badge(status)}
  </div>
  <div class="progress-bar-container" style="margin-bottom:12px">
    <div class="progress-label"><span>Progress</span><span>{denom_display}</span></div>
    <div class="progress-bar"><div class="progress-bar-fill workflow-progress" style="width:{min(progress_pct, 100.0):.1f}%"></div></div>
  </div>
  {method_items if method_items else ''}
  {case_items_html}
  <div class="list-label">Evidence</div>
  <div class="native-proof-links">{evidence_links}</div>
  <div class="provenance-full">
    <strong>Data Provenance</strong>
    <div>Source function: <code>{escape(src_func)}</code> @ <code>{escape(src_file)}</code></div>
    <div>Evidence: <code>{escape(src_evidence)}</code></div>
    <div>Claims: <code>{escape(src_claims)}</code></div>
  </div>
</section>
""".strip()


def _render_layer3_gate_sections(families: list[dict[str, Any]], *, assembly_name: str, root_prefix: str) -> str:
    """Render all Layer 3 gate detail sections for a DLL detail page."""
    sections = ""
    for family in families:
        family_id = _string(family.get("familyId") or "")
        dname = _string(family.get("displayName"))
        native_proof = dict(family.get("nativeProof") or {})
        managed_proof = dict(family.get("managedProof") or {})
        hotupdate_proof = dict(family.get("hotupdateProof") or {})
        benchmark_proof = dict(family.get("benchmarkProof") or {})
        test_code_proof = dict(family.get("testCodeProof") or {})
        audit_input_proof = dict(family.get("auditInputProof") or {})
        codegen_review_proof = dict(family.get("codegenReviewProof") or {})

        # Section header for family grouping
        sections += f'<div class="gate-section-header" style="margin-top:24px"><h3 style="color:var(--muted)">{escape(dname)}</h3></div>'

        for gate_code, gate_label, gate_proof in (
            ("native-proof", "Native Proof", native_proof),
            ("managed-proof", "Managed Proof", managed_proof),
            ("hotupdate-proof", "HotUpdate Proof", hotupdate_proof),
            ("benchmark", "Benchmark", benchmark_proof),
            ("test-code", "Test Code", test_code_proof),
            ("audit-input-and-ledger", "Audit Input & Ledger", audit_input_proof),
            ("codegen-review", "CodeGen Review", codegen_review_proof),
        ):
            sections += _render_gate_detail_section(
                gate_code, gate_label, gate_proof,
                assembly_name=assembly_name,
                family_id=family_id,
                root_prefix=root_prefix,
            )
    return sections


def _heatmap_class(gate_state: str) -> str:
    """Return the heatmap CSS class for a gate execution state."""
    state_map = {
        "passed": "heatmap-cell-passed",
        "completed": "heatmap-cell-passed",
        "passing": "heatmap-cell-passed",
        "in-progress": "heatmap-cell-in-progress",
        "blocked": "heatmap-cell-blocked",
        "pending": "heatmap-cell-pending",
        "not-required": "heatmap-cell-not-required",
        "missing-evidence": "heatmap-cell-missing-evidence",
        "not-started": "heatmap-cell-pending",
    }
    return state_map.get(gate_state, "")


def _render_dashboard(payload: dict[str, Any], *, root_prefix: str, has_ledger: bool = False) -> str:
    program = dict(payload.get("program") or {})
    summary = dict(program.get("summary") or {})
    matrix_rows = list(dict(payload.get("dllMatrix") or {}).get("rows") or [])

    extra_keys: list[str] = []
    if has_ledger and any(row.get("capabilityClosure") is not None for row in matrix_rows):
        extra_keys = ["denominatorStatus", "capabilityClosure", "workflowProgress"]
    gate_keys = [
        key
        for key in list(matrix_rows[0].keys() if matrix_rows else [])
        if key not in {"assemblyName", "orderIndex", "dllState", "currentProject", "riskTags", "denominatorStatus", "capabilityClosure", "workflowProgress"}
    ]
    matrix_headers = ["assemblyName", "dllState", "currentProject", *extra_keys, *gate_keys]

    # --- Matrix body with heatmap + expandable rows ---
    matrix_body = ""
    for row in matrix_rows:
        asm = str(row.get("assemblyName") or "")
        cells = "".join(
            (
                f'<td><a href="./{escape(_dll_detail_relative_path(asm), quote=True)}">{escape(asm)}</a></td>'
                if header == "assemblyName"
                else (
                    f"<td>{_render_mini_bar(row.get('capabilityClosure', {}).get('closurePercent', 0.0), row.get('capabilityClosure', {}).get('closedFamilies', 0), row.get('capabilityClosure', {}).get('totalFamilies', 0))}</td>"
                    if header == "capabilityClosure"
                    else (
                        f"<td>{_render_mini_bar(row.get('workflowProgress', {}).get('progressPercent', 0.0), row.get('workflowProgress', {}).get('passedGates', 0), row.get('workflowProgress', {}).get('totalRequiredGates', 0))}</td>"
                        if header == "workflowProgress"
                        else f'<td class="{_heatmap_class(str(row.get(header, "")))}">{escape(str(row.get(header, "")))}</td>'
                    )
                )
            )
            for header in matrix_headers
        )
        # Expandable detail row: show current project and blocking reason
        detail_html = f"<td colspan=\"{len(matrix_headers)}\"><strong>Blocking:</strong> {escape(str(row.get('blockingReason') or 'n/a'))}</td>"
        matrix_body += (
            f'<tr class="matrix-row-expandable" onclick="toggleRow(this)">{cells}</tr>\n'
            f'<tr class="matrix-row-detail"><td colspan="{len(matrix_headers)}">'
            f'<div style="display:flex;gap:20px;flex-wrap:wrap">'
            f'<div><strong>Current Project:</strong> {escape(str(row.get("currentProject") or "n/a"))}</div>'
            f'<div><strong>Risk Tags:</strong> {escape(", ".join(row.get("riskTags") or []) or "none")}</div>'
            f'<div><strong>Denominator:</strong> {escape(str(row.get("denominatorStatus") or "n/a"))}</div>'
            f'</div>'
            f'</td></tr>\n'
        )

    # --- Detail cards (unchanged, reused in Overview tab) ---
    detail_cards = []
    for dll in list(payload.get("dlls") or []):
        if has_ledger and dll.get("capabilityClosure") is not None:
            cc = dict(dll.get("capabilityClosure") or {})
            ws = dict(dll.get("waiverSummary") or {})
            sl = dict(dll.get("sourceLinks") or {})
            sl_links = ""
            for key, label in [("subjectSource", "Source"), ("generatedCode", "Generated Code"), ("evidence", "Evidence")]:
                path = _string(sl.get(key))
                if path:
                    sl_links += f'<a class="pill-link" href="{escape(root_prefix + _normalized(path), quote=True)}" target="_blank" rel="noreferrer">{escape(label)}</a>'
            waiver_line = ""
            if ws.get("totalWaivers", 0) > 0 or ws.get("totalExclusions", 0) > 0:
                waiver_line = f'<div class="waiver-summary"><span class="waiver-badge active">&#9888; {ws.get("activeWaivers", 0)} active</span><span class="waiver-badge exclusion">&#8855; {ws.get("totalExclusions", 0)} excluded</span></div>'
            detail_cards.append(
                f'''
<div class="dll-card" id="{escape(_status_class(dll.get("assemblyName")), quote=True)}">
  <div class="dll-card-header">
    <div>
      <h3>{escape(str(dll.get("assemblyName") or ""))}</h3>
      <p>{escape(str(dll.get("blockingReason") or "n/a"))}</p>
    </div>
    <div class="dll-card-links">
      {_status_badge(dll.get("dllState"))}
      <a class="pill-link" href="./{escape(_dll_detail_relative_path(str(dll.get("assemblyName") or "")), quote=True)}">Detail Page</a>
      <a class="pill-link" href="./{escape(_dll_json_relative_path(str(dll.get("assemblyName") or "")), quote=True)}">JSON</a>
    </div>
  </div>
  {f'<div class="dll-card-links">{sl_links}</div>' if sl_links else ''}
  {waiver_line}
  <div class="meta-grid">
    <div class="meta-card"><strong>Current Project</strong>{escape(str(dll.get("currentProject") or ""))}</div>
    <div class="meta-card"><strong>Closure</strong>{cc.get("closedFamilies", 0)}/{cc.get("totalFamilies", 0)} families</div>
    <div class="meta-card"><strong>Phase</strong>{escape(str(dll.get("phase") or ""))}</div>
    <div class="meta-card"><strong>Evidence Count</strong>{_dll_evidence_count(dll)}</div>
  </div>
</div>'''.strip()
            )
        else:
            project_cards_html = "".join(_render_project_card(project, root_prefix=root_prefix) for project in list(dll.get("projects") or []))
            detail_cards.append(
                f'''
<div class="dll-card" id="{escape(_status_class(dll.get("assemblyName")), quote=True)}">
  <div class="dll-card-header">
    <div>
      <h3>{escape(str(dll.get("assemblyName") or ""))}</h3>
      <p>{escape(str(dll.get("blockingReason") or "n/a"))}</p>
    </div>
    <div class="dll-card-links">
      {_status_badge(dll.get("dllState"))}
      <a class="pill-link" href="./{escape(_dll_detail_relative_path(str(dll.get("assemblyName") or "")), quote=True)}">Detail Page</a>
      <a class="pill-link" href="./{escape(_dll_json_relative_path(str(dll.get("assemblyName") or "")), quote=True)}">JSON</a>
    </div>
  </div>
  <div class="meta-grid">
    <div class="meta-card"><strong>Current Project</strong>{escape(str(dll.get("currentProject") or ""))}</div>
    <div class="meta-card"><strong>Phase</strong>{escape(str(dll.get("phase") or ""))}</div>
    <div class="meta-card"><strong>Roadmap Task</strong>{escape(str(dll.get("roadmapTaskId") or ""))}</div>
    <div class="meta-card"><strong>Evidence Count</strong>{_dll_evidence_count(dll)}</div>
    <div class="meta-card"><strong>Support Refs</strong>{_dll_support_count(dll)}</div>
    <div class="meta-card"><strong>Verification Projects</strong>{len(list(dll.get("projects") or []))}</div>
  </div>
  <div class="project-grid">{project_cards_html}</div>
</div>'''.strip()
            )

    # --- Overview tab: progress bars + summary cards + detail cards ---
    triple_axis_html = ""
    if has_ledger:
        dll_comp = dict(summary.get("dllCompletion") or {})
        cap_closure = dict(summary.get("capabilityClosure") or {})
        wf_progress = dict(summary.get("workflowProgress") or {})
        dll_count = summary.get("dllCount", 0) or 1
        triple_axis_html = (
            '<div class="progress-axis">'
            + _render_progress_bar("DLL Completion", dll_comp.get("completedDllCount", 0), dll_count, round((dll_comp.get("completedDllCount", 0) / dll_count) * 100, 1), css_class="dll-completion")
            + _render_progress_bar("Capability Closure", cap_closure.get("closedFamilies", 0), cap_closure.get("totalFamilies", 0), cap_closure.get("closurePercent", 0.0), css_class="capability-closure")
            + _render_progress_bar("Workflow Progress", wf_progress.get("passedGates", 0), wf_progress.get("totalRequiredGates", 0), wf_progress.get("progressPercent", 0.0), css_class="workflow-progress")
            + '</div>'
        )

    overview_html = f"""
<div id="tab-overview" class="tab-content active">
  <div class="summary-grid">
    <div class="summary-card"><strong>DLL Count</strong>{escape(str(summary.get("dllCount") or 0))}</div>
    <div class="summary-card"><strong>Completed</strong>{escape(str(summary.get("completedCount") or 0))}</div>
    <div class="summary-card"><strong>In Progress</strong>{escape(str(summary.get("inProgressCount") or 0))}</div>
    <div class="summary-card"><strong>Blocked</strong>{escape(str(summary.get("blockedCount") or 0))}</div>
    <div class="summary-card"><strong>Not Started</strong>{escape(str(summary.get("notStartedCount") or 0))}</div>
    <div class="summary-card"><strong>Active Assembly</strong>{escape(str(summary.get("activeAssembly") or ""))}</div>
  </div>
  {triple_axis_html}
  <section>
    <h2>Program Overview</h2>
    <p>{escape(str(program.get("title") or ""))}</p>
    <div class="meta-grid">
      <div class="meta-card"><strong>Roadmap</strong>{_dashboard_link(str(program.get("roadmapPath") or ""), root_prefix=root_prefix)}</div>
      <div class="meta-card"><strong>Design</strong>{_dashboard_link(str(program.get("designPath") or ""), root_prefix=root_prefix)}</div>
      <div class="meta-card"><strong>Status</strong>{_dashboard_link(str(program.get("statusPath") or ""), root_prefix=root_prefix)}</div>
      <div class="meta-card"><strong>Subject Entry</strong>{_dashboard_link(str(program.get("subjectEntry") or ""), root_prefix=root_prefix)}</div>
    </div>
  </section>
  <section>
    <h2>DLL Detail</h2>
    {"".join(detail_cards)}
  </section>
</div>
"""

    # --- Matrix tab: full matrix with heatmap + expandable rows ---
    matrix_html = f"""
<div id="tab-matrix" class="tab-content">
  <section>
    <h2>DLL Matrix</h2>
    <p>Full verification matrix for all DLLs. Click a row to expand details. Cell colors indicate gate state: green=passed, blue=in-progress, red=blocked, gray=pending, light gray=not-required, orange=missing-evidence.</p>
    <div class="table-wrap">
      <table>
        <thead><tr>{"".join(f"<th>{escape(header)}</th>" for header in matrix_headers)}</tr></thead>
        <tbody>{matrix_body}</tbody>
      </table>
    </div>
  </section>
</div>
"""

    # --- Families tab: per-DLL family closure summary ---
    family_summary_cards = ""
    for dll in list(payload.get("dlls") or []):
        asm = _string(dll.get("assemblyName"))
        families = list(dll.get("capabilityFamilies") or [])
        cc = dict(dll.get("capabilityClosure") or {})
        if not families:
            continue
        total = cc.get("totalFamilies", len(families))
        closed = cc.get("closedFamilies", sum(1 for f in families if f.get("closureStatus") == "closed"))
        in_prog = cc.get("inProgressFamilies", sum(1 for f in families if f.get("closureStatus") == "in-progress"))
        waived = cc.get("waivedFamilies", sum(1 for f in families if f.get("closureStatus") == "waived"))
        excluded = cc.get("excludedFamilies", sum(1 for f in families if f.get("closureStatus") == "excluded"))
        blocked = cc.get("platformBlockedFamilies", sum(1 for f in families if f.get("closureStatus") == "platform-blocked"))
        closed_pct = round((closed / total) * 100, 1) if total > 0 else 0
        family_summary_cards += f"""
<div class="family-summary-card">
  <strong>{escape(asm)}</strong>
  <div>Total Families: {total}</div>
  <div>Closed: {closed} ({closed_pct}%)</div>
  <div>In Progress: {in_prog} | Blocked: {blocked}</div>
  <div>Waived: {waived} | Excluded: {excluded}</div>
  <div class="progress-bar" style="margin-top:8px;height:8px"><div class="progress-bar-fill capability-closure" style="width:{min(closed_pct,100.0)}%;height:100%"></div></div>
</div>"""
    # Overall summary
    total_families_all = sum(dll.get("capabilityClosure", {}).get("totalFamilies", 0) for dll in payload.get("dlls", []) if dll.get("capabilityFamilies"))
    closed_all = sum(dll.get("capabilityClosure", {}).get("closedFamilies", 0) for dll in payload.get("dlls", []) if dll.get("capabilityFamilies"))
    in_prog_all = sum(dll.get("capabilityClosure", {}).get("inProgressFamilies", 0) for dll in payload.get("dlls", []) if dll.get("capabilityFamilies"))
    waived_all = sum(dll.get("capabilityClosure", {}).get("waivedFamilies", 0) for dll in payload.get("dlls", []) if dll.get("capabilityFamilies"))
    closed_all_pct = round((closed_all / total_families_all) * 100, 1) if total_families_all > 0 else 0
    overall_family_header = f"""
<div class="family-summary-card" style="border-left:4px solid var(--accent);background:var(--accent-soft)">
  <strong>All DLLs Combined</strong>
  <div>Total Families: {total_families_all}</div>
  <div>Closed: {closed_all} ({closed_all_pct}%) | In Progress: {in_prog_all} | Waived: {waived_all}</div>
  <div class="progress-bar" style="margin-top:8px;height:12px"><div class="progress-bar-fill capability-closure" style="width:{min(closed_all_pct,100.0)}%;height:100%"></div></div>
</div>""" if total_families_all > 0 else ""

    families_html = f"""
<div id="tab-families" class="tab-content">
  <section>
    <h2>Families Overview</h2>
    <p>Per-DLL capability family closure status. Each card shows the closure distribution for one assembly's families.</p>
    <div class="family-summary-grid">
      {overall_family_header}
      {family_summary_cards}
    </div>
  </section>
</div>
"""

    # --- Gates tab: per-gate global progress ---
    gate_progress_data: dict[str, dict[str, int]] = {}
    for gate_code in GATE_COLUMNS:
        gate_label = GATE_LABELS.get(gate_code, gate_code)
        gate_progress_data[gate_label] = {"passed": 0, "total": 0, "blocked": 0, "pending": 0}
    for dll in list(payload.get("dlls") or []):
        for family in list(dll.get("capabilityFamilies") or []):
            gates = dict(family.get("verificationGates") or {})
            for gate_code in GATE_COLUMNS:
                gl = GATE_LABELS.get(gate_code, gate_code)
                state = _string(gates.get(gate_code))
                if state and state != "not-required":
                    gate_progress_data[gl]["total"] += 1
                    if state == "passed":
                        gate_progress_data[gl]["passed"] += 1
                    elif state in ("blocked", "failed"):
                        gate_progress_data[gl]["blocked"] += 1
                    else:
                        gate_progress_data[gl]["pending"] += 1

    gate_progress_cards = ""
    for gl in sorted(gate_progress_data.keys()):
        gd = gate_progress_data[gl]
        passed_pct = round((gd["passed"] / gd["total"]) * 100, 1) if gd["total"] > 0 else 0
        gate_progress_cards += f"""
<div class="gate-progress-card">
  <h4>{escape(gl)}</h4>
  <div>Passed: {gd['passed']}/{gd['total']} ({passed_pct}%)</div>
  <div>Blocked: {gd['blocked']} | Pending: {gd['pending']}</div>
  <div class="progress-bar" style="margin-top:8px;height:8px"><div class="progress-bar-fill workflow-progress" style="width:{min(passed_pct,100.0)}%;height:100%"></div></div>
</div>"""

    gates_html = f"""
<div id="tab-gates" class="tab-content">
  <section>
    <h2>Gates Progress</h2>
    <p>Per-gate global progress across all DLLs and families. Shows how many gate evaluations have passed versus total required.</p>
    <div class="gate-progress-grid">
      {gate_progress_cards}
    </div>
  </section>
</div>
"""

    # --- Assemble template ---
    template = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Foundation DLL Audit</title>
  <link rel="stylesheet" href="./dashboard.css">
</head>
<body>
  <main>
    <div class="page-header">
      <div class="eyebrow">Verification Program</div>
      <h1>Foundation DLL Audit</h1>
      <p>Unified view for `System.Private.CoreLib + 13 DLLs`, including DLL status, family detail, verification projects, and evidence entry points.</p>
      <div class="top-nav">
        <a href="./artifact-index.html">Artifact Index</a>
      </div>
      <div class="tab-bar">
        <button class="tab-button active" data-tab="overview" onclick="switchTab('overview')">Overview</button>
        <button class="tab-button" data-tab="matrix" onclick="switchTab('matrix')">Matrix</button>
        <button class="tab-button" data-tab="families" onclick="switchTab('families')">Families</button>
        <button class="tab-button" data-tab="gates" onclick="switchTab('gates')">Gates</button>
      </div>
    </div>
    {overview_html}
    {matrix_html}
    {families_html}
    {gates_html}
  </main>
  {_dashboard_scripts()}
</body>
</html>
"""
    return template


def _summary_markdown(payload: dict[str, Any]) -> str:
    program = dict(payload.get("program") or {})
    summary = dict(program.get("summary") or {})
    lines = [
        "# Foundation DLL Audit Summary",
        "",
        f"- programId: {program.get('programId') or ''}",
        f"- title: {program.get('title') or ''}",
        f"- dllCount: {summary.get('dllCount') or 0}",
        f"- completedCount: {summary.get('completedCount') or 0}",
        f"- inProgressCount: {summary.get('inProgressCount') or 0}",
        f"- blockedCount: {summary.get('blockedCount') or 0}",
        f"- notStartedCount: {summary.get('notStartedCount') or 0}",
        f"- activeAssembly: {summary.get('activeAssembly') or ''}",
        "- dashboard: docs/verification/foundation-dll-audit/dashboard.html",
        "",
        "## DLLs",
    ]
    for dll in list(payload.get("dlls") or []):
        lines.append(
            f"- {dll.get('assemblyName')}: state={dll.get('dllState')} currentProject={dll.get('currentProject')} detail=docs/verification/foundation-dll-audit/{_dll_detail_relative_path(str(dll.get('assemblyName') or ''))}"
        )
    return "\n".join(lines).strip() + "\n"


def _render_artifact_index_page(payload: dict[str, Any], *, root_prefix: str) -> str:
    artifact_headers = ["assemblyName", "projectCode", "displayName", "path", "artifactKind", "linkTargetType", "role", "exists"]
    artifact_body = "".join(
        "<tr>"
        + "".join(
            (
                f"<td>{_dashboard_link(str(row.get(header) or ''), root_prefix=root_prefix)}</td>"
                if header == "path"
                else f"<td>{escape(str(row.get(header, '')))}</td>"
            )
            for header in artifact_headers
        )
        + "</tr>"
        for row in list(dict(payload.get("artifactIndex") or {}).get("rows") or [])
    )
    return f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Artifact Index - Foundation DLL Audit</title>
  <link rel="stylesheet" href="./dashboard.css">
</head>
<body>
  <main>
    <div class="page-header">
      <a class="back-link" href="./dashboard.html">Back To Dashboard</a>
      <div class="eyebrow">Appendix</div>
      <h1>Artifact Index</h1>
      <p>All artifacts referenced across foundation DLL audit projections.</p>
      <div class="table-wrap">
        <table>
          <thead><tr>{"".join(f"<th>{escape(h)}</th>" for h in artifact_headers)}</tr></thead>
          <tbody>{artifact_body}</tbody>
        </table>
      </div>
    </div>
  </main>
</body>
</html>"""


def _write_projection_bundle(repo_root: Path, payload: dict[str, Any], *, output_root: Path, has_ledger: bool = False) -> list[str]:
    output_root.mkdir(parents=True, exist_ok=True)
    dll_root = output_root / "dlls"
    dll_root.mkdir(parents=True, exist_ok=True)
    has_ledger = has_ledger or bool(payload.get("program", {}).get("summary", {}).get("capabilityClosure"))
    files = {
        "program": output_root / "program.json",
        "familyVerification": output_root / "family-verification.json",
        "familyVerificationClaims": output_root / "family-verification-claims.json",
        "truthContracts": output_root / "truth-contracts.json",
        "dllMatrix": output_root / "dll-matrix.json",
        "artifactIndex": output_root / "artifact-index.json",
        "dashboard": output_root / "dashboard.html",
        "dashboardCss": output_root / "dashboard.css",
        "summary": output_root / "summary.md",
    }
    files["artifactIndexHtml"] = output_root / "artifact-index.html"
    write_json(files["program"], payload["program"])
    write_json(files["familyVerification"], payload["familyVerification"])
    write_json(files["familyVerificationClaims"], payload["familyVerificationClaims"])
    write_json(files["truthContracts"], payload["truthContracts"])
    write_json(files["dllMatrix"], payload["dllMatrix"])
    write_json(files["artifactIndex"], payload["artifactIndex"])
    files["dashboardCss"].write_text(_dashboard_styles(), encoding="utf-8")
    files["dashboard"].write_text(
        _render_dashboard(payload, root_prefix=_root_relative_prefix(repo_root, files["dashboard"]), has_ledger=has_ledger),
        encoding="utf-8",
    )
    if has_ledger:
        files["artifactIndexHtml"].write_text(
            _render_artifact_index_page(payload, root_prefix=_root_relative_prefix(repo_root, files["artifactIndexHtml"])),
            encoding="utf-8",
        )
    files["summary"].write_text(_summary_markdown(payload), encoding="utf-8")

    artifacts = [_relative(repo_root, path) for path in files.values()]
    for dll in list(payload.get("dlls") or []):
        detail_json_path = dll_root / f"{_string(dll.get('assemblyName'))}.json"
        detail_html_path = dll_root / f"{_string(dll.get('assemblyName'))}.html"
        write_json(detail_json_path, dll)
        detail_html_path.write_text(
            _render_dll_detail_page(dll, repo_root=repo_root, root_prefix=_root_relative_prefix(repo_root, detail_html_path)),
            encoding="utf-8",
        )
        artifacts.append(_relative(repo_root, detail_json_path))
        artifacts.append(_relative(repo_root, detail_html_path))
    return artifacts


def write_foundation_dll_audit_outputs(repo_root: Path) -> dict[str, Any]:
    payload = build_foundation_dll_audit_payload(repo_root)
    has_ledger = bool(payload.get("program", {}).get("summary", {}).get("capabilityClosure"))
    projection_root = verification_layout_module.foundation_dll_audit_projection_root(repo_root)
    docs_root = verification_layout_module.docs_foundation_dll_audit_root(repo_root)
    report_root = verification_layout_module.archive_report_scope_root(
        repo_root,
        closure_kind="completed",
        scope_code="foundation-dll-audit",
    )
    report_root.mkdir(parents=True, exist_ok=True)
    report_summary_path = report_root / "summary.md"
    report_summary_path.write_text(_summary_markdown(payload), encoding="utf-8")

    artifacts = _write_projection_bundle(repo_root, payload, output_root=projection_root, has_ledger=has_ledger)
    artifacts.extend(_write_projection_bundle(repo_root, payload, output_root=docs_root, has_ledger=has_ledger))
    artifacts.append(_relative(repo_root, report_summary_path))
    artifacts = list(dict.fromkeys(artifacts))
    return {
        "outputRoot": _relative(repo_root, projection_root),
        "dashboardPath": _relative(repo_root, projection_root / "dashboard.html"),
        "docsOutputRoot": _relative(repo_root, docs_root),
        "docsDashboardPath": _relative(repo_root, docs_root / "dashboard.html"),
        "reportSummaryPath": _relative(repo_root, report_summary_path),
        "artifacts": artifacts,
        "program": payload["program"],
    }
