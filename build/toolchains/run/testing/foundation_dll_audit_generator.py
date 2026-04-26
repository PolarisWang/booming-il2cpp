from __future__ import annotations

import json
import re
import sys
from html import escape
from pathlib import Path
from typing import Any

try:
    from ..core.common import write_json
    from . import verification_layout as verification_layout_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json
    from testing import verification_layout as verification_layout_module


PROGRAM_MANIFEST_RELATIVE_PATH = (
    "verification",
    "catalog",
    "programs",
    "foundation-dll-translation-audit.program.json",
)
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
    return json.loads(path.read_text(encoding="utf-8"))


def _load_capability_ledger(repo_root: Path) -> dict[str, Any] | None:
    path = repo_root.joinpath(*LEDGER_RELATIVE_PATH)
    if not path.is_file():
        return None
    return json.loads(path.read_text(encoding="utf-8"))


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
    task_ids = [
        _string(assembly_entry.get("roadmapTaskId")),
        *[_string(item) for item in _list(assembly_entry.get("followupTaskIds")) if _string(item)],
    ]
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
    for assembly_entry in sorted(
        list(program_manifest.get("assemblies") or []),
        key=lambda item: int(item.get("orderIndex") or 0),
    ):
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
            "phase": _string(assembly_entry.get("phase")),
            "roadmapTaskId": _string(assembly_entry.get("roadmapTaskId")),
            "dllState": dll_state,
            "currentProject": current_project,
            "blockingReason": blocker_summary,
            "riskTags": list(assembly_entry.get("riskTags") or []),
            "projects": projects,
        }
        if has_ledger:
            ledger_entry = ledger_lookup.get(assembly_name)
            families = list((ledger_entry or {}).get("families") or [])
            family_source = ""
            if not families:
                families = [_auto_derive_family(assembly_name, projects, project_templates)]
                dll_record["familySource"] = "auto-derived"
                family_source = "auto-derived"
            dll_record["capabilityClosure"] = _compute_capability_closure(families)
            dll_record["workflowProgress"] = _compute_gate_progress(families, project_templates=project_templates)
            dll_record["capabilityFamilies"] = families
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
  --bg: #f3efe6;
  --bg-strong: #e2d6c0;
  --panel: #fffaf2;
  --panel-strong: #fff4df;
  --line: #d6cab4;
  --ink: #202523;
  --muted: #5e675f;
  --accent: #98553a;
  --accent-soft: #f2dfd4;
  --ok-bg: #dff4e4;
  --ok-ink: #205b35;
  --warn-bg: #ffe7c7;
  --warn-ink: #8a4f14;
  --pending-bg: #ece6de;
  --pending-ink: #5f5347;
  --blocked-bg: #f5d9d4;
  --blocked-ink: #8b3124;
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
.status-completed { background: var(--ok-bg); color: var(--ok-ink); }
.status-blocked { background: var(--blocked-bg); color: var(--blocked-ink); }
.status-pending,
.status-conditional,
.status-not-started,
.status-required { background: var(--pending-bg); color: var(--pending-ink); }
.status-in-progress { background: #deedf9; color: #24527a; }
.status-failed { background: var(--blocked-bg); color: var(--blocked-ink); }
.status-warning { background: var(--warn-bg); color: var(--warn-ink); }
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
/* dll detail dual axis */
.dll-axis { margin-top: 0; }
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


def _render_source_links_block(sl: dict[str, Any], *, root_prefix: str) -> str:
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
        resolved = root_prefix + _normalized(path)
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
    "audit-input-and-ledger": "Audit Input",
    "managed-proof": "Managed Proof",
    "native-proof": "Native Proof",
    "hotupdate-proof": "HotUpdate",
    "benchmark": "Benchmark",
    "codegen-review": "CodeGen Review",
}
GATE_COLUMNS = tuple(GATE_LABELS.keys())


def _family_has_active_gates(gates: dict[str, str]) -> bool:
    """Check if a family has any non-pending, non-not-required gate activity."""
    for k, v in gates.items():
        if k == "completion-certification":
            continue
        v = _string(v)
        if v and v not in ("pending", "not-required", ""):
            return True
    return False


def _render_family_table(families: list[dict[str, Any]], *, root_prefix: str) -> str:
    if not families:
        return ""
    rows = ""
    for idx, family in enumerate(families, start=1):
        dname = _string(family.get("displayName"))
        status = _string(family.get("closureStatus"))
        gates = dict(family.get("verificationGates") or {})
        mc = family.get("methodCount", 0)
        test_code = dict(family.get("testCode") or {})
        test_code_status = _string(test_code.get("testCodeStatus"))
        test_code_cell = _status_badge(test_code_status) if test_code_status else '<span class="status-badge status-muted">n/a</span>'
        gate_badges = "".join(f"<td>{_status_badge(gates.get(col, ''))}</td>" for col in GATE_COLUMNS)
        bold_class = ' class="family-active"' if _family_has_active_gates(gates) else ""
        rows += f"<tr{bold_class}><td>{idx}</td><td>{escape(dname)}</td><td>{_status_badge(status)}</td><td>{test_code_cell}</td>{gate_badges}<td>{mc}</td></tr>"
    gate_headers = "".join(f"<th>{GATE_LABELS[col]}</th>" for col in GATE_COLUMNS)
    return f"""
<section>
  <h2>Capability Families</h2>
  <p>Each capability family represents a logical group of methods. A family is considered closed when all its non-exempt verification gates pass.</p>
  <div class="table-wrap">
    <table class="family-table">
      <thead><tr><th>#</th><th>Family</th><th>closureStatus</th><th>Test Code</th>{gate_headers}<th>methodCount</th></tr></thead>
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


def _render_dll_detail_page(dll: dict[str, Any], *, root_prefix: str) -> str:
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

    family_section = _render_family_table(families, root_prefix=root_prefix)
    waiver_section = _render_waiver_table(families)
    source_links_html = _render_source_links_block(sl, root_prefix=root_prefix)

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
      <p>按 DLL 审核当前验证状态、capability closure、验证项目与证据。</p>
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
      {waiver_section}
    </div>
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
  </main>
</body>
</html>
"""


def _render_dashboard(payload: dict[str, Any], *, root_prefix: str, has_ledger: bool = False) -> str:
    program = dict(payload.get("program") or {})
    summary = dict(program.get("summary") or {})
    matrix_rows = list(dict(payload.get("dllMatrix") or {}).get("rows") or [])

    extra_keys: list[str] = []
    if has_ledger and any(row.get("capabilityClosure") is not None for row in matrix_rows):
        extra_keys = ["denominatorStatus", "capabilityClosure", "workflowProgress"]
    matrix_headers = [
        "assemblyName",
        "dllState",
        "currentProject",
        *extra_keys,
        *[
            key
            for key in list(matrix_rows[0].keys() if matrix_rows else [])
            if key not in {"assemblyName", "orderIndex", "dllState", "currentProject", "riskTags", "denominatorStatus", "capabilityClosure", "workflowProgress"}
        ],
    ]

    matrix_body = "".join(
        "<tr>"
        + "".join(
            (
                f"<td>{_local_link('./' + _dll_detail_relative_path(str(row.get(header) or '')), str(row.get(header) or ''))}</td>"
                if header == "assemblyName"
                else (
                    f"<td>{_render_mini_bar(row.get('capabilityClosure', {}).get('closurePercent', 0.0), row.get('capabilityClosure', {}).get('closedFamilies', 0), row.get('capabilityClosure', {}).get('totalFamilies', 0))}</td>"
                    if header == "capabilityClosure"
                    else (
                        f"<td>{_render_mini_bar(row.get('workflowProgress', {}).get('progressPercent', 0.0), row.get('workflowProgress', {}).get('passedGates', 0), row.get('workflowProgress', {}).get('totalRequiredGates', 0))}</td>"
                        if header == "workflowProgress"
                        else f"<td>{escape(str(row.get(header, '')))}</td>"
                    )
                )
            )
            for header in matrix_headers
        )
        + "</tr>"
        for row in matrix_rows
    )

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
            project_cards = "".join(_render_project_card(project, root_prefix=root_prefix) for project in list(dll.get("projects") or []))
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
  <div class="project-grid">{project_cards}</div>
</div>'''.strip()
            )

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
      <p>围绕 `System.Private.CoreLib + 13 DLL` 输出统一的总览、DLL 细项、验证项目和证据入口。</p>
      <div class="top-nav">
        <a href="#program">Program Overview</a>
        <a href="#matrix">DLL Matrix</a>
        <a href="#details">DLL Detail</a>
        <a href="./artifact-index.html">Artifact Index</a>
      </div>
      <div class="summary-grid">
        <div class="summary-card"><strong>DLL Count</strong>{escape(str(summary.get("dllCount") or 0))}</div>
        <div class="summary-card"><strong>Completed</strong>{escape(str(summary.get("completedCount") or 0))}</div>
        <div class="summary-card"><strong>In Progress</strong>{escape(str(summary.get("inProgressCount") or 0))}</div>
        <div class="summary-card"><strong>Blocked</strong>{escape(str(summary.get("blockedCount") or 0))}</div>
        <div class="summary-card"><strong>Not Started</strong>{escape(str(summary.get("notStartedCount") or 0))}</div>
        <div class="summary-card"><strong>Active Assembly</strong>{escape(str(summary.get("activeAssembly") or ""))}</div>
      </div>
      {triple_axis_html}
    </div>
    <section id="program">
      <h2>Program Overview</h2>
      <p>{escape(str(program.get("title") or ""))}</p>
      <div class="meta-grid">
        <div class="meta-card"><strong>Roadmap</strong>{_dashboard_link(str(program.get("roadmapPath") or ""), root_prefix=root_prefix)}</div>
        <div class="meta-card"><strong>Design</strong>{_dashboard_link(str(program.get("designPath") or ""), root_prefix=root_prefix)}</div>
        <div class="meta-card"><strong>Status</strong>{_dashboard_link(str(program.get("statusPath") or ""), root_prefix=root_prefix)}</div>
        <div class="meta-card"><strong>Subject Entry</strong>{_dashboard_link(str(program.get("subjectEntry") or ""), root_prefix=root_prefix)}</div>
      </div>
    </section>
    <section id="matrix">
      <h2>DLL Matrix</h2>
      <div class="table-wrap">
        <table>
          <thead><tr>{"".join(f"<th>{escape(header)}</th>" for header in matrix_headers)}</tr></thead>
          <tbody>{matrix_body}</tbody>
        </table>
      </div>
    </section>
    <section id="details">
      <h2>DLL Detail</h2>
      {"".join(detail_cards)}
    </section>
  </main>
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
        "dllMatrix": output_root / "dll-matrix.json",
        "artifactIndex": output_root / "artifact-index.json",
        "dashboard": output_root / "dashboard.html",
        "dashboardCss": output_root / "dashboard.css",
        "summary": output_root / "summary.md",
    }
    files["artifactIndexHtml"] = output_root / "artifact-index.html"
    write_json(files["program"], payload["program"])
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
            _render_dll_detail_page(dll, root_prefix=_root_relative_prefix(repo_root, detail_html_path)),
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
