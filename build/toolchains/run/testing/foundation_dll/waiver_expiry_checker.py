"""Waiver Expiry Checker — automated waiver expiration for principle violations.

Principle waivers let families pass with CONCERN/VIOLATION when there's a
documented reason. But waivers must have expiry dates — once expired, the
waiver no longer exempts the family from principle alignment requirements.

This checker is called by the aggregate step to determine if any waivers
have expired and should block closureStatus.

Usage:
    python waiver_expiry_checker.py                                          # check defaults
    python waiver_expiry_checker.py --waivers path/to/principle-waivers.json
    python waiver_expiry_checker.py --block-expired                           # exit 1 if expired exist
    python waiver_expiry_checker.py --days 30                                 # warn 30 days before expiry
"""

from __future__ import annotations

import json
import sys
from datetime import date, datetime
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_DEFAULT_WAIVER_PATH = (
    _REPO_ROOT / "verification" / "projections" / "foundation-dll-audit" / "principle-waivers.json"
)


def load_waivers(path: Path | str) -> dict[str, Any]:
    """Load principle-waivers.json. Returns empty dict if not found."""
    p = Path(path)
    if not p.exists():
        return {"waivers": [], "error": f"File not found: {p}"}
    try:
        return json.loads(p.read_bytes())
    except (json.JSONDecodeError, OSError) as e:
        return {"waivers": [], "error": str(e)}


def check_expiry(
    waiver_path: Path | str | None = None,
    warn_days: int = 30,
    block_expired: bool = False,
) -> dict[str, Any]:
    """Check all waivers for expiry.

    Returns:
        {
            "valid": True/False,
            "total_waivers": N,
            "expired": [...],
            "expiring_soon": [...within warn_days...],
            "valid_waivers": [...],
        }
    """
    path = Path(waiver_path) if waiver_path else _DEFAULT_WAIVER_PATH
    data = load_waivers(path)
    today = date.today()

    expired: list[dict] = []
    expiring_soon: list[dict] = []
    valid: list[dict] = []

    for waiver in data.get("waivers", []):
        rd_str = waiver.get("reviewDate", "") or waiver.get("expiryDate", "")
        if not rd_str:
            expired.append({**waiver, "_reason": "no review date"})
            continue

        try:
            rd = date.fromisoformat(rd_str)
        except ValueError:
            expired.append({**waiver, "_reason": f"invalid date: {rd_str}"})
            continue

        if rd < today:
            expired.append({**waiver, "_reason": f"expired {rd_str}"})
        elif (rd - today).days <= warn_days:
            expiring_soon.append({**waiver, "_reason": f"expires {rd_str} ({ (rd - today).days } days)"})
        else:
            valid.append(waiver)

    result = {
        "checked_at": datetime.now().isoformat(),
        "waiver_path": str(path),
        "total_waivers": len(data.get("waivers", [])),
        "expired_count": len(expired),
        "expiring_soon_count": len(expiring_soon),
        "valid_count": len(valid),
        "expired": expired,
        "expiring_soon": expiring_soon,
        "valid": valid,
        "valid_status": len(expired) == 0,
    }

    return result


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Waiver Expiry Checker")
    parser.add_argument("--waivers", type=Path, help="Path to principle-waivers.json")
    parser.add_argument("--block-expired", action="store_true",
                        help="Exit with code 1 if expired waivers exist")
    parser.add_argument("--days", type=int, default=30,
                        help="Days threshold for 'expiring soon' warning")
    args = parser.parse_args()

    result = check_expiry(
        waiver_path=args.waivers,
        warn_days=args.days,
        block_expired=args.block_expired,
    )

    print(json.dumps(result, indent=2, ensure_ascii=False))

    if args.block_expired and result.get("expired_count", 0) > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
