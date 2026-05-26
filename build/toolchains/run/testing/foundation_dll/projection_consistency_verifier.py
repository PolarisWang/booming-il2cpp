from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def verify_projection_consistency(repo_root: Path, *, mode: str = "quick") -> dict[str, Any]:
    projection_root = repo_root / "verification" / "projections" / "foundation-dll-audit"
    docs_root = repo_root / "docs" / "verification" / "foundation-dll-audit"

    program = json.loads((projection_root / "program.json").read_text(encoding="utf-8"))
    matrix = json.loads((projection_root / "dll-matrix.json").read_text(encoding="utf-8"))
    checks: list[dict[str, Any]] = []

    checks.append(
        {
            "name": "program-has-capability-closure",
            "status": "ok" if int(program["summary"]["capabilityClosure"]["totalFamilies"]) > 0 else "fail",
        }
    )
    checks.append(
        {
            "name": "matrix-row-count",
            "status": "ok" if len(matrix.get("rows", [])) == len(program.get("scopeAssemblies", [])) else "fail",
        }
    )
    checks.append(
        {
            "name": "docs-dashboard-exists",
            "status": "ok" if (docs_root / "dashboard.html").is_file() else "fail",
        }
    )

    if mode == "full":
        for assembly in program.get("scopeAssemblies", []):
            checks.append(
                {
                    "name": f"dll-detail-{assembly}",
                    "status": "ok" if (projection_root / "dlls" / f"{assembly}.json").is_file() else "fail",
                }
            )

    validated = all(check["status"] == "ok" for check in checks)
    return {
        "mode": mode,
        "validated": validated,
        "checks": checks,
    }
