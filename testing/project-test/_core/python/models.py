"""Project context model — complete .NET application verification.

A project is a real multi-file .NET application that goes through the
full il2cpp pipeline: managed -> IL2CPP codegen -> native build -> execute.
Verification checks application-level behavior (stdout, exit code)
rather than per-method Subject_N pass/fail.
"""

from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

from _pipeline.base_context import ContextBase
from _pipeline.base_report import ReportBase


@dataclass
class ProjectContext(ContextBase):
    """Context for a complete project verification run.

    Path resolution uses project_dir (alias of root_dir) as root.
    """

    # ── project_dir is an alias of root_dir ──
    @property
    def project_dir(self) -> Path:
        return self.root_dir

    @project_dir.setter
    def project_dir(self, value: Path) -> None:
        self.root_dir = value

    @property
    def contract_path(self) -> Path:
        return self.project_dir / "project.json"

    @property
    def managed_dir(self) -> Path:
        return self.project_dir / "managed" / "src"

    @property
    def native_dir(self) -> Path:
        return self.project_dir / "native"

    @property
    def expected_dir(self) -> Path:
        return self.project_dir / "expected"

    @property
    def patch_dir(self) -> Path:
        return self.project_dir / "patch"

    @property
    def codegen_dir(self) -> Path:
        return self.project_dir / "codegen"

    @property
    def entry_exe_path(self) -> Path:
        return self.native_dir / "entry-aot.exe"

    @property
    def entry_jit_exe_path(self) -> Path:
        return self.native_dir / "entry-jit.exe"

    @property
    def results_dir(self) -> Path:
        return self.project_dir.parents[2] / "results" / "project-test" / self.slug

    # ── Helpers ────────────────────────────────────────────────────────

    def load_project_meta(self) -> dict[str, Any]:
        """Load and return the project.json content."""
        meta_path = self.contract_path
        if not meta_path.exists():
            return {}
        try:
            return json.loads(meta_path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            return {}

    @property
    def entry_point(self) -> str:
        return self.load_project_meta().get("entryPoint", "")


@dataclass
class ProjectReport(ReportBase):
    """Report for a complete project verification run."""
    fact_output_match: bool = False
    hotupdate_summary: dict[str, Any] = field(default_factory=dict)
    benchmark_summary: dict[str, Any] = field(default_factory=dict)

    def to_json(self) -> str:
        import json
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)
