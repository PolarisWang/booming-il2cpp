from __future__ import annotations

from typing import Any


def match_family_candidates(synthesized: list[dict[str, Any]], existing: list[dict[str, Any]]) -> dict[str, Any]:
    synthesized_ids = [str(item.get("familyId") or "") for item in synthesized]
    existing_ids = [str(item.get("familyId") or "") for item in existing]

    matched = [family_id for family_id in synthesized_ids if family_id in existing_ids]
    new_candidates = [family_id for family_id in synthesized_ids if family_id not in existing_ids]
    orphan_confirmed = [family_id for family_id in existing_ids if family_id not in synthesized_ids]

    return {
        "matchedFamilyIds": matched,
        "newCandidateFamilyIds": new_candidates,
        "orphanConfirmedFamilyIds": orphan_confirmed,
        "mergeSuspectFamilyIds": [],
        "splitSuspectFamilyIds": [],
    }
