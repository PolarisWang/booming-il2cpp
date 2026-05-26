from __future__ import annotations

from pathlib import Path


VERIFICATION_ROOT_NAME = "artifact/verification-catalog"
OWNER_MANIFEST_NAME = "owner.manifest.json"
OWNER_FEATURES_NAME = "owner.features.json"
WORKSPACE_MANIFEST_NAME = "workspace.manifest.json"
VERIFICATION_ALL_MANIFEST_NAME = "verification-all.manifest.json"


def verification_root(repo_root: Path) -> Path:
    return repo_root / VERIFICATION_ROOT_NAME


def verification_manifest_path(repo_root: Path) -> Path:
    return verification_root(repo_root) / "verification.manifest.json"


def verification_index_path(repo_root: Path) -> Path:
    return verification_root(repo_root) / "INDEX.md"


def catalog_root(repo_root: Path) -> Path:
    return verification_root(repo_root) / "catalog"


def owners_root(repo_root: Path) -> Path:
    return catalog_root(repo_root) / "owners"


def owner_root(repo_root: Path, owner_id: str) -> Path:
    return owners_root(repo_root) / str(owner_id).strip()


def owner_manifest_path(repo_root: Path, owner_id: str) -> Path:
    return owner_root(repo_root, owner_id) / OWNER_MANIFEST_NAME


def owner_features_path(repo_root: Path, owner_id: str) -> Path:
    return owner_root(repo_root, owner_id) / OWNER_FEATURES_NAME


def owner_proofs_root(repo_root: Path, owner_id: str) -> Path:
    return owner_root(repo_root, owner_id) / "proofs"


def owner_benchmarks_root(repo_root: Path, owner_id: str) -> Path:
    return owner_root(repo_root, owner_id) / "benchmarks"


def owner_support_root(repo_root: Path, owner_id: str) -> Path:
    return owner_root(repo_root, owner_id) / "support"


def owner_support_host_root(repo_root: Path, owner_id: str) -> Path:
    return owner_support_root(repo_root, owner_id) / "host"


def owner_support_interpreter_root(repo_root: Path, owner_id: str) -> Path:
    return owner_support_root(repo_root, owner_id) / "interpreter"


def owner_support_patch_root(repo_root: Path, owner_id: str) -> Path:
    return owner_support_root(repo_root, owner_id) / "patch"


def owner_support_shared_contracts_root(repo_root: Path, owner_id: str) -> Path:
    return owner_support_root(repo_root, owner_id) / "shared-contracts"


def owner_benchmark_baselines_root(repo_root: Path, owner_id: str) -> Path:
    return owner_root(repo_root, owner_id) / "benchmark-baselines"


def scenarios_root(repo_root: Path) -> Path:
    return catalog_root(repo_root) / "scenarios"


def scenario_owner_root(repo_root: Path, owner_id: str) -> Path:
    return scenarios_root(repo_root) / str(owner_id).strip()


def archive_root(repo_root: Path) -> Path:
    return verification_root(repo_root) / "archive"


def archive_latest_root(repo_root: Path) -> Path:
    return archive_root(repo_root) / "latest"


def archive_master_root(repo_root: Path) -> Path:
    return archive_root(repo_root) / "master"


def archive_reports_root(repo_root: Path) -> Path:
    return archive_root(repo_root) / "reports"


def archive_report_scope_root(repo_root: Path, *, closure_kind: str, scope_code: str) -> Path:
    return archive_reports_root(repo_root) / str(closure_kind).strip() / str(scope_code).strip()


def evidence_root(repo_root: Path) -> Path:
    return verification_root(repo_root) / "evidence"


def evidence_owners_root(repo_root: Path) -> Path:
    return evidence_root(repo_root) / "owners"


def owner_evidence_root(repo_root: Path, owner_id: str) -> Path:
    return evidence_owners_root(repo_root) / str(owner_id).strip()


def owner_codegen_stubs_root(repo_root: Path, owner_id: str) -> Path:
    return owner_evidence_root(repo_root, owner_id) / "codegen-stubs"


def projections_root(repo_root: Path) -> Path:
    return verification_root(repo_root) / "projections"


def testing_inventory_projection_root(repo_root: Path) -> Path:
    return projections_root(repo_root) / "testing-inventory"


def benchmark_projection_root(repo_root: Path) -> Path:
    return projections_root(repo_root) / "benchmark"


def foundation_dll_audit_projection_root(repo_root: Path) -> Path:
    return projections_root(repo_root) / "foundation-dll-audit"


def docs_verification_root(repo_root: Path) -> Path:
    return repo_root / "docs" / "verification"


def docs_foundation_dll_audit_root(repo_root: Path) -> Path:
    return docs_verification_root(repo_root) / "foundation-dll-audit"


def workspaces_root(repo_root: Path) -> Path:
    return verification_root(repo_root) / "workspaces"


def verification_all_solution_path(repo_root: Path) -> Path:
    return workspaces_root(repo_root) / "verification-all.sln"


def verification_all_manifest_path(repo_root: Path) -> Path:
    return workspaces_root(repo_root) / VERIFICATION_ALL_MANIFEST_NAME


def verification_all_report_path(repo_root: Path) -> Path:
    return workspaces_root(repo_root) / "verification-all.generation.report.json"


def subject_workspaces_root(repo_root: Path) -> Path:
    return workspaces_root(repo_root) / "subjects"


def subject_workspace_root(repo_root: Path, subject_id: str) -> Path:
    return subject_workspaces_root(repo_root) / str(subject_id).strip()


def subject_workspace_manifest_path(repo_root: Path, subject_id: str) -> Path:
    return subject_workspace_root(repo_root, subject_id) / WORKSPACE_MANIFEST_NAME


def core_workspaces_root(repo_root: Path) -> Path:
    return workspaces_root(repo_root) / "core"


def core_workspace_root(repo_root: Path, host_platform: str) -> Path:
    return core_workspaces_root(repo_root) / str(host_platform).strip()


def core_workspace_manifest_path(repo_root: Path, host_platform: str) -> Path:
    return core_workspace_root(repo_root, host_platform) / WORKSPACE_MANIFEST_NAME


def raw_benchmark_records_root(repo_root: Path) -> Path:
    return repo_root / "artifact" / "verification" / "benchmark-records"


def raw_benchmark_records_path(repo_root: Path, owner_id: str) -> Path:
    return raw_benchmark_records_root(repo_root) / str(owner_id).strip() / "records.jsonl"


def raw_stress_records_root(repo_root: Path) -> Path:
    return repo_root / "artifact" / "verification" / "stress-records"


def raw_stress_records_path(repo_root: Path, test_name: str) -> Path:
    return raw_stress_records_root(repo_root) / str(test_name).strip() / "records.jsonl"


def repo_root_from_owner_manifest(manifest_path: Path) -> Path:
    resolved = manifest_path.resolve()
    for parent in resolved.parents:
        if parent.name == "owners" and parent.parent.name == "catalog" and parent.parent.parent.name == Path(VERIFICATION_ROOT_NAME).name:
            return parent.parent.parent.parent
        if parent.name == "subjects":
            fixtures_parent = parent.parent
            if fixtures_parent.name == "fixtures" and fixtures_parent.parent.name == "tests":
                return fixtures_parent.parent.parent
            return fixtures_parent
    raise ValueError(
        f"owner manifest path must be rooted under verification/catalog/owners or tests/fixtures/subjects: {manifest_path}"
    )


def owner_manifest_name_candidates() -> tuple[str, ...]:
    return (OWNER_MANIFEST_NAME,)


def owner_features_name_candidates() -> tuple[str, ...]:
    return (OWNER_FEATURES_NAME,)


def owner_scan_root(repo_root: Path, owner_id: str, source_path_text: str) -> Path:
    resolved_source_path = Path(source_path_text)
    if not resolved_source_path.is_absolute():
        resolved_source_path = repo_root / resolved_source_path

    resolved_owner_root = owner_root(repo_root, owner_id)
    try:
        if resolved_source_path == resolved_owner_root or resolved_owner_root in resolved_source_path.parents:
            return resolved_owner_root
    except ValueError:
        pass

    return resolved_source_path if resolved_source_path.is_dir() else resolved_source_path.parent
