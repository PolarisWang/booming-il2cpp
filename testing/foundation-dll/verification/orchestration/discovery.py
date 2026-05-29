"""Family discovery — shared by all verification entry points.

Usage:
    from verification.orchestration.discovery import discover_families, SKIP_SLUGS
"""

from __future__ import annotations

from pathlib import Path

# Families excluded from batch runs (optimization campaigns, etc.)
SKIP_SLUGS: set[str] = {
    "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
    "interface-dispatch", "pinvoke-dllimport", "primitive-numeric-conversions-core",
}


def discover_families(assembly: str = "System.Private.CoreLib",
                      verbose: bool = False,
                      families_dir: Path | None = None) -> list[str]:
    """Scan an assembly directory for family contract files.

    Args:
        assembly: Assembly name (default: System.Private.CoreLib).
        verbose: Log skip reasons.
        families_dir: Override the base directory (for testing).

    Returns:
        Sorted list of family slugs that have a valid contract.
    """
    if families_dir is None:
        from verification._path import _HERE as _VERIFICATION_ROOT
        families_dir = _VERIFICATION_ROOT.parent / assembly

    if not families_dir.is_dir():
        print(f"WARNING: Assembly directory not found: {families_dir}")
        return []

    all_dirs = sorted(d for d in families_dir.iterdir() if d.is_dir())
    slugs: list[str] = []

    for d in all_dirs:
        has_cap = (d / "capability-family-contract.json").exists()
        has_legacy = (d / "contract.json").exists()
        slug = d.name

        if not has_cap and not has_legacy:
            if verbose:
                print(f"  [discover] skip {slug}/ — no contract file found")
            continue

        if slug in SKIP_SLUGS:
            if verbose:
                print(f"  [discover] skip {slug}/ — in SKIP_SLUGS")
            continue

        if has_legacy and not has_cap:
            if verbose:
                print(f"  [discover] {slug}/ — using legacy contract.json (not yet migrated)")
        elif verbose:
            print(f"  [discover] {slug}/ — capability-family-contract.json")
        slugs.append(slug)

    return slugs
