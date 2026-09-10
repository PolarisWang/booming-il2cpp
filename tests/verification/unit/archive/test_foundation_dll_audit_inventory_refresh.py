from __future__ import annotations

import shutil

from tests._support.fs import make_temp_repo_root
from tests._support.module_loading import load_module
from tests.verification.unit.projections.foundation_dll.test_foundation_dll_audit_generator import (
    _write_fixture_repo,
)


REPO_ROOT = __import__("pathlib").Path(__file__).resolve().parents[4]
INVENTORY_GENERATOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "inventory_generator.py"


def test_inventory_refresh_also_materializes_foundation_dll_audit_projection_and_index_entry() -> None:
    generator_module = load_module(INVENTORY_GENERATOR_MODULE_PATH, "chaos_inventory_generator_foundation_dll_audit")
    repo_root = make_temp_repo_root("verification", "inventory-foundation-dll-audit")
    _write_fixture_repo(repo_root)

    try:
        payload = generator_module.write_inventory_outputs(
            repo_root,
            host_platform="windows",
            output_root=repo_root / "verification" / "projections" / "testing-inventory",
        )

        assert (repo_root / "verification" / "projections" / "foundation-dll-audit" / "dashboard.html").is_file()
        assert (
            repo_root / "verification" / "archive" / "reports" / "completed" / "foundation-dll-audit" / "summary.md"
        ).is_file()
        assert payload["foundationDllAudit"]["outputRoot"] == "verification/projections/foundation-dll-audit"
        assert payload["foundationDllAudit"]["reportSummaryPath"] == (
            "verification/archive/reports/completed/foundation-dll-audit/summary.md"
        )
        assert "verification/projections/foundation-dll-audit/dashboard.html" in payload["artifacts"]

        verification_index = (repo_root / "verification" / "INDEX.md").read_text(encoding="utf-8")
        assert "Foundation DLL audit" in verification_index
        assert "verification/projections/foundation-dll-audit/dashboard.html" in verification_index
    finally:
        shutil.rmtree(repo_root, ignore_errors=True)
