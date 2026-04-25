from __future__ import annotations

import json
import shutil
from pathlib import Path

from tests._support.fs import make_temp_repo_root, write_json
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
GENERATOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll_audit_generator.py"


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def _program_manifest() -> dict:
    return {
        "schemaVersion": 1,
        "programId": "foundation-dll-translation-audit",
        "title": "Foundation DLL Translation Audit",
        "roadmapPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/roadmap-v1-01.md",
        "designPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md",
        "statusPath": "docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md",
        "subjectEntry": "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln",
        "projectTemplates": [
            {
                "code": "audit-input-and-ledger",
                "displayName": "Audit Input And Ledger",
                "defaultPolicyState": "required",
                "verificationMethod": "刷新 audit input manifest，并校验 surface / semantic / nativeization plan。",
                "artifactKeywords": ["input-manifest", "surface-ledger", "semantic-ledger", "nativeization-plan", "audit"],
            },
            {
                "code": "managed-proof",
                "displayName": "Managed Proof",
                "defaultPolicyState": "required",
                "verificationMethod": "运行 managed proof，验证语义基线。",
                "artifactKeywords": ["managed-proof", "proof"],
            },
            {
                "code": "native-proof",
                "displayName": "Native Proof",
                "defaultPolicyState": "required",
                "verificationMethod": "运行 native / AOT proof，验证 generated native code 可执行。",
                "artifactKeywords": ["native-proof", "native-aot", "native-reference", "runtime-skeleton", "generated/runtime"],
            },
            {
                "code": "hotupdate-proof",
                "displayName": "HotUpdate Proof",
                "defaultPolicyState": "conditional",
                "verificationMethod": "运行 patch / host proof，验证 hotupdate consumer 路径。",
                "artifactKeywords": ["hotupdate-proof", "hot-update", "hotupdate"],
            },
            {
                "code": "benchmark",
                "displayName": "Benchmark",
                "defaultPolicyState": "conditional",
                "verificationMethod": "运行 benchmark，归档性能结果。",
                "artifactKeywords": ["benchmark"],
            },
            {
                "code": "codegen-review",
                "displayName": "Codegen Review",
                "defaultPolicyState": "required",
                "verificationMethod": "审核 generated code、dispatch/helper/binding/coverage。",
                "artifactKeywords": ["codegen-review", "generated/audit", "generated/runtime"],
            },
            {
                "code": "completion-certification",
                "displayName": "Completion Certification",
                "defaultPolicyState": "required",
                "verificationMethod": "汇总前置 project 结果，给出 DLL completion 结论。",
                "artifactKeywords": ["completion-report", "completion", "STATUS.md"],
            },
        ],
        "assemblies": [
            {
                "assemblyName": "System.Private.CoreLib",
                "orderIndex": 1,
                "phase": "phase-1",
                "roadmapTaskId": "20260419-03-system-private-corelib-full-verification",
                "followupTaskIds": [
                    "20260419-20-corelib-supplemental-full-closure-audit-integration",
                    "20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening",
                ],
                "riskTags": ["core-foundation"],
                "projectPolicies": {
                    "hotupdate-proof": "required",
                    "benchmark": "conditional",
                },
            },
            {
                "assemblyName": "System.Collections.Immutable",
                "orderIndex": 2,
                "phase": "phase-2",
                "roadmapTaskId": "20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable",
                "riskTags": [],
                "projectPolicies": {
                    "hotupdate-proof": "conditional",
                    "benchmark": "conditional",
                },
            },
        ],
    }


def _write_fixture_repo(repo_root: Path) -> None:
    write_json(
        repo_root / "verification" / "catalog" / "programs" / "foundation-dll-translation-audit.program.json",
        _program_manifest(),
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-01-foundation-dll-translation-audit-roadmap" / "design-v1-01.md",
        "# design\n",
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-01-foundation-dll-translation-audit-roadmap" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-01-foundation-dll-translation-audit-roadmap",
                "title: Foundation DLL Translation Audit Roadmap",
                "lifecycle_status: in-progress",
                "phase: roadmap",
                "---",
                "",
                "## 当前判断",
                "",
                "- DLL-first 审核与报告模型已拍板。",
                "",
            ]
        )
        + "\n",
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-01-foundation-dll-translation-audit-roadmap" / "roadmap-v1-01.md",
        "\n".join(
            [
                "# Roadmap",
                "",
                "| task_id | phase | status | owner | purpose | depends_on |",
                "| --- | --- | --- | --- | --- | --- |",
                "| `20260419-03-system-private-corelib-full-verification` | `phase-1` | `completed-blocked` | `codex` | CoreLib full verification remains blocked by translated coverage gaps. | `20260419-02` |",
                "| `20260419-20-corelib-supplemental-full-closure-audit-integration` | `phase-1` | `completed` | `codex` | Supplemental full-closure evidence connected. | `20260419-07` |",
                "| `20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening` | `phase-1` | `in-progress` | `codex` | Continue widening translated coverage. | `20260419-20` |",
                "| `20260419-21-complex-bcl-ordered-dll-01-system-collections-immutable` | `phase-2` | `blocked` | `codex` | Waits for CoreLib full verification. | `20260419-03` |",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "docs" / "dev" / "completed" / "20260419-03-system-private-corelib-full-verification" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-03-system-private-corelib-full-verification",
                "title: System.Private.CoreLib Full Verification",
                "---",
                "",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/hot-update/`",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "docs" / "dev" / "completed" / "20260419-20-corelib-supplemental-full-closure-audit-integration" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-20-corelib-supplemental-full-closure-audit-integration",
                "title: CoreLib Supplemental Full Closure Audit Integration",
                "---",
                "",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/audit/System.Private.CoreLib/managed-proof-report.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/audit/System.Private.CoreLib/codegen-review.json`",
                "- `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/audit/System.Private.CoreLib/codegen-review.md`",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "docs" / "dev" / "in-progress" / "20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening" / "STATUS.md",
        "\n".join(
            [
                "---",
                "task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening",
                "title: CoreLib Supplemental Runtime Skeleton Coverage Widening",
                "---",
                "",
                "next_step: Continue widening translated coverage.",
                "",
            ]
        ),
    )
    _write_text(
        repo_root / "subjects" / "SolutionCorePack" / "source" / "EngineeringScenarios" / "FoundationDllTranslationSolution" / "FoundationDllTranslationSolution.sln",
        "Microsoft Visual Studio Solution File\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "managed-proof-report.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "codegen-review.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "codegen-review.md",
        "# review\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "analysis" / "generated" / "supplemental-full-closures" / "system-private-corelib" / "native-reference" / "generated" / "runtime" / "native-reference.runtime-skeleton.coverage.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "analysis" / "generated" / "supplemental-full-closures" / "system-private-corelib" / "native-aot" / "native-aot.plan.json",
        "{}\n",
    )
    _write_text(
        repo_root / "artifacts" / "subjects" / "SolutionCorePack" / "runs" / "20260424-200729-windows-23a1" / "audit" / "System.Private.CoreLib" / "hotupdate-proof-report.json",
        "{}\n",
    )


def test_write_foundation_dll_audit_outputs_materializes_program_matrix_and_report_files() -> None:
    generator_module = load_module(GENERATOR_MODULE_PATH, "chaos_foundation_dll_audit_generator_outputs")
    repo_root = make_temp_repo_root("verification", "foundation-dll-audit")
    _write_fixture_repo(repo_root)

    try:
        payload = generator_module.write_foundation_dll_audit_outputs(repo_root)

        projection_root = repo_root / "verification" / "projections" / "foundation-dll-audit"
        report_root = repo_root / "verification" / "archive" / "reports" / "completed" / "foundation-dll-audit"
        docs_root = repo_root / "docs" / "verification" / "foundation-dll-audit"
        assert (projection_root / "program.json").is_file()
        assert (projection_root / "dll-matrix.json").is_file()
        assert (projection_root / "artifact-index.json").is_file()
        assert (projection_root / "dashboard.html").is_file()
        assert (projection_root / "dashboard.css").is_file()
        assert (projection_root / "dlls" / "System.Private.CoreLib.json").is_file()
        assert (projection_root / "dlls" / "System.Private.CoreLib.html").is_file()
        assert (report_root / "summary.md").is_file()
        assert (docs_root / "program.json").is_file()
        assert (docs_root / "dll-matrix.json").is_file()
        assert (docs_root / "artifact-index.json").is_file()
        assert (docs_root / "dashboard.html").is_file()
        assert (docs_root / "dashboard.css").is_file()
        assert (docs_root / "summary.md").is_file()
        assert (docs_root / "dlls" / "System.Private.CoreLib.json").is_file()
        assert (docs_root / "dlls" / "System.Private.CoreLib.html").is_file()

        program_payload = json.loads((projection_root / "program.json").read_text(encoding="utf-8"))
        assert program_payload["programId"] == "foundation-dll-translation-audit"
        assert program_payload["summary"]["dllCount"] == 2
        assert program_payload["summary"]["completedCount"] == 0
        assert program_payload["summary"]["blockedCount"] == 2
        assert program_payload["summary"]["activeAssembly"] == "System.Private.CoreLib"

        matrix_payload = json.loads((projection_root / "dll-matrix.json").read_text(encoding="utf-8"))
        rows = {row["assemblyName"]: row for row in matrix_payload["rows"]}
        assert rows["System.Private.CoreLib"]["dllState"] == "blocked"
        assert rows["System.Private.CoreLib"]["currentProject"] == "completion-certification"
        assert rows["System.Collections.Immutable"]["dllState"] == "blocked"

        detail_payload = json.loads(
            (projection_root / "dlls" / "System.Private.CoreLib.json").read_text(encoding="utf-8")
        )
        projects = {item["projectCode"]: item for item in detail_payload["projects"]}
        assert projects["managed-proof"]["executionState"] == "passed"
        assert projects["native-proof"]["executionState"] == "passed"
        assert projects["codegen-review"]["executionState"] == "passed"
        assert projects["completion-certification"]["executionState"] == "blocked"
        assert projects["managed-proof"]["supportRefs"]
        assert all(path.startswith("docs/") or path.startswith("subjects/") for path in projects["managed-proof"]["supportRefs"])
        assert any(
            artifact["path"].endswith("managed-proof-report.json") and artifact["linkTargetType"] == "file"
            for artifact in projects["managed-proof"]["artifacts"]
        )
        assert all(path.startswith("artifacts/") for path in (artifact["path"] for artifact in projects["managed-proof"]["artifacts"]))

        immutable_payload = json.loads(
            (projection_root / "dlls" / "System.Collections.Immutable.json").read_text(encoding="utf-8")
        )
        immutable_projects = {item["projectCode"]: item for item in immutable_payload["projects"]}
        assert immutable_projects["audit-input-and-ledger"]["executionState"] == "blocked"
        assert immutable_projects["managed-proof"]["executionState"] == "pending"
        assert immutable_projects["native-proof"]["executionState"] == "pending"
        assert immutable_projects["completion-certification"]["executionState"] == "blocked"
        assert immutable_projects["managed-proof"]["artifacts"] == []
        assert immutable_projects["managed-proof"]["supportRefs"]

        artifact_payload = json.loads((projection_root / "artifact-index.json").read_text(encoding="utf-8"))
        assert any(
            item["assemblyName"] == "System.Private.CoreLib"
            and item["projectCode"] == "native-proof"
            and item["linkTargetType"] == "file"
            for item in artifact_payload["rows"]
        )
        assert all(not item["path"].startswith("docs/") for item in artifact_payload["rows"])
        assert all(not item["path"].startswith("subjects/") for item in artifact_payload["rows"])
        dashboard_html = (projection_root / "dashboard.html").read_text(encoding="utf-8")
        assert "{{" not in dashboard_html
        assert 'href="./dashboard.css"' in dashboard_html
        assert 'href="./dlls/System.Private.CoreLib.html"' in dashboard_html
        assert '<div class="dll-card"' in dashboard_html
        assert '<section id="details">' in dashboard_html
        assert "Support Refs" in dashboard_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in dashboard_html
        projection_dll_html = (projection_root / "dlls" / "System.Private.CoreLib.html").read_text(encoding="utf-8")
        assert 'href="../dashboard.css"' in projection_dll_html
        assert "Verification Projects" in projection_dll_html
        assert "Managed Proof" in projection_dll_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in projection_dll_html
        docs_dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
        assert "{{" not in docs_dashboard_html
        assert 'href="./dashboard.css"' in docs_dashboard_html
        assert 'href="./dlls/System.Private.CoreLib.html"' in docs_dashboard_html
        assert '<div class="dll-card"' in docs_dashboard_html
        assert '<section id="details">' in docs_dashboard_html
        assert "Support Refs" in docs_dashboard_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in docs_dashboard_html
        docs_dll_html = (docs_root / "dlls" / "System.Private.CoreLib.html").read_text(encoding="utf-8")
        assert 'href="../dashboard.css"' in docs_dll_html
        assert "Verification Projects" in docs_dll_html
        assert "Managed Proof" in docs_dll_html
        assert "../../../artifacts/subjects/SolutionCorePack/runs/" in docs_dll_html
        assert "verification/projections/foundation-dll-audit/dashboard.html" in payload["artifacts"]
        assert "verification/projections/foundation-dll-audit/dashboard.css" in payload["artifacts"]
        assert "verification/projections/foundation-dll-audit/dlls/System.Private.CoreLib.html" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/dashboard.html" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/dashboard.css" in payload["artifacts"]
        assert "docs/verification/foundation-dll-audit/dlls/System.Private.CoreLib.html" in payload["artifacts"]
        assert payload["reportSummaryPath"] == "verification/archive/reports/completed/foundation-dll-audit/summary.md"
        assert payload["docsDashboardPath"] == "docs/verification/foundation-dll-audit/dashboard.html"
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
