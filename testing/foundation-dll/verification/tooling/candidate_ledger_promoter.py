from __future__ import annotations

import json
from pathlib import Path
from typing import Any


CANDIDATE_LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.candidate.json")
LEDGER_PATH = ("verification", "projections", "foundation-dll-audit", "capability-family-ledger.json")


def promote_candidate_ledger(repo_root: Path, *, scope: str = "all", promote_approved: bool = False) -> dict[str, Any]:
    if not promote_approved:
        raise RuntimeError("promote-approved flag is required")

    candidate_path = repo_root.joinpath(*CANDIDATE_LEDGER_PATH)
    payload = json.loads(candidate_path.read_text(encoding="utf-8"))
    payload["snapshotId"] = "snap-20260426-audit-confirmed-v1"
    payload["denominatorStatus"] = "audit-confirmed"

    promoted_dlls: list[str] = []
    for dll in payload.get("dlls", []):
        if scope != "all" and dll.get("assemblyName") != scope:
            continue
        dll["denominatorStatus"] = "audit-confirmed"
        promoted_dlls.append(str(dll.get("assemblyName") or ""))
        for family in dll.get("families", []):
            family["denominatorStatus"] = "audit-confirmed"

    output_path = repo_root.joinpath(*LEDGER_PATH)
    output_path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return {
        "ledgerPath": output_path.relative_to(repo_root).as_posix(),
        "promotedDlls": promoted_dlls,
        "snapshotId": payload["snapshotId"],
    }
