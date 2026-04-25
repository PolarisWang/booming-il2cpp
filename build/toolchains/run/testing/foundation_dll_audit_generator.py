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
        matrix_rows.append(matrix_row)
    return {
        "program": program_payload,
        "dlls": dll_rows,
        "dllMatrix": {
            "schemaVersion": 1,
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
      <p>按 DLL 审核当前验证状态、验证项目、证据与支持引用。</p>
      <div class="summary-grid">
        <div class="summary-card"><strong>State</strong>{_status_badge(dll.get("dllState"))}</div>
        <div class="summary-card"><strong>Current Project</strong>{escape(str(dll.get("currentProject") or ""))}</div>
        <div class="summary-card"><strong>Phase</strong>{escape(str(dll.get("phase") or ""))}</div>
        <div class="summary-card"><strong>Roadmap Task</strong>{escape(str(dll.get("roadmapTaskId") or ""))}</div>
        <div class="summary-card"><strong>Evidence Count</strong>{_dll_evidence_count(dll)}</div>
        <div class="summary-card"><strong>Support Refs</strong>{_dll_support_count(dll)}</div>
      </div>
      <div class="link-grid">
        <div class="link-card"><strong>JSON Payload</strong>{_local_link(f"./{_string(dll.get('assemblyName'))}.json")}</div>
        <div class="link-card"><strong>Dashboard</strong>{_local_link("../dashboard.html", "foundation-dll-audit/dashboard.html")}</div>
      </div>
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


def _render_dashboard(payload: dict[str, Any], *, root_prefix: str) -> str:
    program = dict(payload.get("program") or {})
    summary = dict(program.get("summary") or {})
    matrix_rows = list(dict(payload.get("dllMatrix") or {}).get("rows") or [])
    matrix_headers = [
        "assemblyName",
        "dllState",
        "currentProject",
        *[
            key
            for key in list(matrix_rows[0].keys() if matrix_rows else [])
            if key not in {"assemblyName", "orderIndex", "dllState", "currentProject", "riskTags"}
        ],
    ]

    matrix_body = "".join(
        "<tr>"
        + "".join(
            (
                f"<td>{_local_link('./' + _dll_detail_relative_path(str(row.get(header) or '')), str(row.get(header) or ''))}</td>"
                if header == "assemblyName"
                else f"<td>{escape(str(row.get(header, '')))}</td>"
            )
            for header in matrix_headers
        )
        + "</tr>"
        for row in matrix_rows
    )

    detail_cards = []
    for dll in list(payload.get("dlls") or []):
        project_cards = "".join(
            _render_project_card(project, root_prefix=root_prefix)
            for project in list(dll.get("projects") or [])
        )
        detail_cards.append(
            f"""
<div class="dll-card" id="{escape(_status_class(dll.get('assemblyName')), quote=True)}">
  <div class="dll-card-header">
    <div>
      <h3>{escape(str(dll.get("assemblyName") or ""))}</h3>
      <p>{escape(str(dll.get("blockingReason") or "n/a"))}</p>
    </div>
    <div class="dll-card-links">
      {_status_badge(dll.get("dllState"))}
      <a class="pill-link" href="./{escape(_dll_detail_relative_path(str(dll.get('assemblyName') or '')), quote=True)}">Detail Page</a>
      <a class="pill-link" href="./{escape(_dll_json_relative_path(str(dll.get('assemblyName') or '')), quote=True)}">JSON</a>
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
</div>
            """.strip()
        )

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
        <a href="#artifacts">Artifact Index</a>
      </div>
      <div class="summary-grid">
        <div class="summary-card"><strong>DLL Count</strong>{escape(str(summary.get("dllCount") or 0))}</div>
        <div class="summary-card"><strong>Completed</strong>{escape(str(summary.get("completedCount") or 0))}</div>
        <div class="summary-card"><strong>In Progress</strong>{escape(str(summary.get("inProgressCount") or 0))}</div>
        <div class="summary-card"><strong>Blocked</strong>{escape(str(summary.get("blockedCount") or 0))}</div>
        <div class="summary-card"><strong>Not Started</strong>{escape(str(summary.get("notStartedCount") or 0))}</div>
        <div class="summary-card"><strong>Active Assembly</strong>{escape(str(summary.get("activeAssembly") or ""))}</div>
      </div>
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
    <section id="artifacts">
      <h2>Artifact Index</h2>
      <div class="table-wrap">
        <table>
          <thead><tr>{"".join(f"<th>{escape(header)}</th>" for header in artifact_headers)}</tr></thead>
          <tbody>{artifact_body}</tbody>
        </table>
      </div>
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


def _write_projection_bundle(repo_root: Path, payload: dict[str, Any], *, output_root: Path) -> list[str]:
    output_root.mkdir(parents=True, exist_ok=True)
    dll_root = output_root / "dlls"
    dll_root.mkdir(parents=True, exist_ok=True)
    files = {
        "program": output_root / "program.json",
        "dllMatrix": output_root / "dll-matrix.json",
        "artifactIndex": output_root / "artifact-index.json",
        "dashboard": output_root / "dashboard.html",
        "dashboardCss": output_root / "dashboard.css",
        "summary": output_root / "summary.md",
    }
    write_json(files["program"], payload["program"])
    write_json(files["dllMatrix"], payload["dllMatrix"])
    write_json(files["artifactIndex"], payload["artifactIndex"])
    files["dashboardCss"].write_text(_dashboard_styles(), encoding="utf-8")
    files["dashboard"].write_text(
        _render_dashboard(payload, root_prefix=_root_relative_prefix(repo_root, files["dashboard"])),
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

    artifacts = _write_projection_bundle(repo_root, payload, output_root=projection_root)
    artifacts.extend(_write_projection_bundle(repo_root, payload, output_root=docs_root))
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
