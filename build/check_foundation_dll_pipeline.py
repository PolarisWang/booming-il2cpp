#!/usr/bin/env python3
"""Foundation DLL Pipeline — unified verification via verify-family orchestrator.

Unified pipeline calling the canonical 9-stage verify_family() for each family:

  1. Ledger overview     — per-DLL table (active vs placeholder)
  2. Phase 2 generation  — run_phase2.py
  3. Per-family verify   — verify_family(slug) for each active family
  4. Results table       — aggregate preflight/codegen/fact/audit/overall
  5. Dashboard refresh   — verification-v1 projection refresh

Returns exit code 0 on success, 1 on any family with failed required stages.
"""

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]

LEDGER = REPO / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"
PHASE2 = REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "run_phase2.py"
REFRESH_CMD = [sys.executable, str(REPO / "build" / "toolchains" / "run" / "run.py"),
               "verify", "verification-v1", "--json"]

# Path to the orchestrator module (for importing verify_family)
_ORCHESTRATOR_DIR = REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll"


# ── helpers ──────────────────────────────────────────────────────────────

def _load_ledger():
    """Load the capability-family-ledger and return (dlls_list, lookup_dict)."""
    ledger = json.loads(LEDGER.read_text(encoding="utf-8"))
    dlls = ledger.get("dlls", [])
    lookup = {}
    for dll in dlls:
        aname = dll["assemblyName"]
        slug_map = {}
        for fam in dll.get("families", []):
            fid = fam.get("familyId", "")
            slug = fid.rsplit("/", 1)[-1] if "/" in fid else fid
            slug_map[slug] = fam
        lookup[aname] = slug_map
    return dlls, lookup


def _on_disk_families(aname):
    """Return set of family slugs that have a directory on disk."""
    dll_dir = REPO / "verification" / "foundation-dll" / aname
    if not dll_dir.is_dir():
        return set()
    return {p.name for p in dll_dir.iterdir() if p.is_dir() and p.name != "reports"}


def _print_sep(char="=", width=72):
    print(char * width)


def _print_header(title):
    _print_sep()
    print(f"  {title}")
    _print_sep()


# ── Step 1: Ledger overview ─────────────────────────────────────────────

def step_ledger_overview(dlls, lookup):
    """Print a per-DLL table of active vs placeholder families."""
    _print_header("Step 1: Foundation-DLL Ledger Overview")

    total_families = 0
    total_active = 0
    total_methods = 0
    total_on_disk = 0

    rows = []
    for dll in dlls:
        aname = dll["assemblyName"]
        families = dll.get("families", [])
        fam_count = len(families)
        active = [f for f in families if f.get("methodCount", 0) > 0]
        active_count = len(active)
        method_count = sum(f.get("methodCount", 0) for f in active)
        on_disk = len(_on_disk_families(aname))
        placeholder_count = fam_count - active_count

        total_families += fam_count
        total_active += active_count
        total_methods += method_count
        total_on_disk += on_disk

        status = "ACTIVE" if active_count > 0 else "PLACEHOLDER"
        rows.append((aname, fam_count, active_count, placeholder_count, method_count, on_disk, status))

    header = f"{'DLL':44s} {'Fams':>5s} {'Active':>7s} {'Place':>6s} {'Methods':>8s} {'OnDisk':>7s}  Status"
    print(header)
    print("-" * len(header))
    for r in rows:
        print(f"{r[0]:44s} {r[1]:5d} {r[2]:7d} {r[3]:6d} {r[4]:8d} {r[5]:7d}  {r[6]}")
    print("-" * len(header))
    print(f"{'TOTAL':44s} {total_families:5d} {total_active:7d} {total_families - total_active:6d} {total_methods:8d} {total_on_disk:7d}")

    # Warn about on-disk families missing from ledger
    for dll in dlls:
        aname = dll["assemblyName"]
        ledger_slugs = set(lookup.get(aname, {}).keys())
        disk_slugs = _on_disk_families(aname)
        missing_from_ledger = disk_slugs - ledger_slugs
        if missing_from_ledger:
            print(f"\n  WARNING: {aname} has families on disk not in ledger:")
            for s in sorted(missing_from_ledger):
                print(f"    - {s}")

    print()
    return total_active


# ── Step 2: Phase 2 generation ──────────────────────────────────────────

def step_phase2():
    """Run Phase 2 — generate test/benchmark/host/patch files."""
    _print_header("Step 2: Phase 2 -- Generate test/benchmark/host/patch files")
    t0 = time.time()
    r = subprocess.run(
        [sys.executable, str(PHASE2)],
        cwd=REPO, capture_output=False, text=True,
    )
    elapsed = time.time() - t0
    if r.returncode != 0:
        print(f"\n  Phase 2 FAILED (rc={r.returncode}, {elapsed:.1f}s)")
        return False
    print(f"\n  Phase 2 OK ({elapsed:.1f}s)")
    return True


# ── Step 3: Per-family verification via verify_family() ─────────────────

def _import_verify_family():
    """Import and return the verify_family function from the orchestrator."""
    sys.path.insert(0, str(_ORCHESTRATOR_DIR))
    sys.path.insert(0, str(_ORCHESTRATOR_DIR.parent.parent))
    # Need to also import from the containing package
    from testing.foundation_dll.family_verification_orchestrator import verify_family
    return verify_family


def _get_verifyable_families(dlls, lookup):
    """Return list of (assembly_name, family_slug, family_name) that can be verified.

    A family is verifyable if:
      - It's marked active in the ledger (methodCount > 0)
      - It has an on-disk directory
      - It has a capability-family-contract.json
    """
    candidates = []
    for dll in dlls:
        aname = dll["assemblyName"]
        disk_slugs = _on_disk_families(aname)
        prefix = f"family/{aname}/"
        for fam in dll.get("families", []):
            if fam.get("methodCount", 0) <= 0:
                continue
            fid = fam.get("familyId", "")
            # Normalize familyId to slug: strip "family/AssemblyName/" prefix,
            # then replace "/" back to "-" (ledger stores hyphens as "/").
            if fid.startswith(prefix):
                slug = fid[len(prefix):].replace("/", "-")
            else:
                slug = fid.rsplit("/", 1)[-1] if "/" in fid else fid
            if slug not in disk_slugs:
                continue
            contract_path = REPO / "verification" / "foundation-dll" / aname / slug / "capability-family-contract.json"
            if not contract_path.exists():
                print(f"  SKIP {aname}/{slug} — no contract on disk")
                continue
            candidates.append((aname, slug))
    return candidates


def step_verify_families(families, *, mode, skip_stages, verbose, codegen_mode=None):
    """Run verify_family() for each family.

    Returns dict mapping (assembly, slug) -> unified_report dict.
    """
    _print_header(f"Step 3: Per-Family Verification (mode={mode})")

    verify_family = _import_verify_family()
    results = {}

    if not families:
        print("  (no families to verify)")
        return results

    print(f"  Families to verify: {len(families)}\n")

    for idx, (aname, slug) in enumerate(families):
        print(f"{'─'*60}")
        print(f"  [{idx+1}/{len(families)}] {aname}/{slug}")
        print(f"{'─'*60}")

        t0 = time.time()
        try:
            report = verify_family(
                slug,
                assembly=aname,
                mode=mode,
                skip_stages=skip_stages,
                verbose=verbose,
                codegen_mode=codegen_mode,
            )
        except Exception as e:
            print(f"\n  FAMILY CRASHED: {e}")
            report = {
                "family": slug,
                "assembly": aname,
                "overall_status": "crashed",
                "stages": {},
                "coverage": {},
            }

        elapsed = time.time() - t0
        results[(aname, slug)] = report

        # Print one-line summary for this family
        overall = report.get("overall_status", "unknown")
        stages = report.get("stages", {})
        stage_line = " | ".join(
            f"{s}:{stages[s].get('status','?')[:4]}"
            for s in ["preflight", "codegen", "fact", "audit", "benchmark", "hotupdate"]
            if s in stages
        )
        print(f"\n  => {overall.upper()}  ({elapsed:.1f}s)  [{stage_line}]\n")

    return results


# ── Step 4: Results table ───────────────────────────────────────────────

def step_results_table(dlls, verify_results):
    """Print aggregate results table from verify_family() results."""
    _print_header("Step 4: Results Summary")

    # Per-DLL aggregation
    dll_agg = {}
    for dll in dlls:
        aname = dll["assemblyName"]
        dll_agg[aname] = {
            "total_families": 0,
            "verified": 0,
            "passed": 0,
            "failed": 0,
            "crashed": 0,
            "stages": {"preflight": 0, "codegen": 0, "fact": 0, "audit": 0},
            "stages_ok": {"preflight": 0, "codegen": 0, "fact": 0, "audit": 0},
        }

    for (aname, slug), report in verify_results.items():
        if aname not in dll_agg:
            dll_agg[aname] = {
                "total_families": 0, "verified": 0, "passed": 0, "failed": 0, "crashed": 0,
                "stages": {}, "stages_ok": {},
            }
        dll_agg[aname]["total_families"] += 1
        dll_agg[aname]["verified"] += 1

        overall = report.get("overall_status", "unknown")
        if overall == "passed":
            dll_agg[aname]["passed"] += 1
        elif overall == "crashed":
            dll_agg[aname]["crashed"] += 1
        else:
            dll_agg[aname]["failed"] += 1

        stages = report.get("stages", {})
        for s in ["preflight", "codegen", "fact", "audit"]:
            if s in stages:
                dll_agg[aname]["stages"].setdefault(s, 0)
                dll_agg[aname]["stages"][s] += 1
                if stages[s].get("status") == "passed":
                    dll_agg[aname]["stages_ok"].setdefault(s, 0)
                    dll_agg[aname]["stages_ok"][s] += 1

    # Print per-DLL table
    header = (f"{'DLL':44s} {'Fams':>5s} {'Vrfy':>5s} {'Pass':>5s} {'Fail':>5s} "
              f"{'Pre':>4s} {'Cgn':>4s} {'Fact':>5s} {'Audit':>5s}")
    print(header)
    print("-" * len(header))
    total_fams = total_vrfy = total_pass = total_fail = 0
    for aname in sorted(dll_agg):
        r = dll_agg[aname]
        if r["total_families"] == 0:
            continue
        total_fams += r["total_families"]
        total_vrfy += r["verified"]
        total_pass += r["passed"]
        total_fail += r["failed"] + r["crashed"]
        pre_ok = r["stages_ok"].get("preflight", 0)
        cgn_ok = r["stages_ok"].get("codegen", 0)
        fact_ok = r["stages_ok"].get("fact", 0)
        audit_ok = r["stages_ok"].get("audit", 0)
        pre_n = r["stages"].get("preflight", 0)
        cgn_n = r["stages"].get("codegen", 0)
        fact_n = r["stages"].get("fact", 0)
        audit_n = r["stages"].get("audit", 0)
        pre_s = f"{pre_ok}/{pre_n}" if pre_n > 0 else "-"
        cgn_s = f"{cgn_ok}/{cgn_n}" if cgn_n > 0 else "-"
        fact_s = f"{fact_ok}/{fact_n}" if fact_n > 0 else "-"
        audit_s = f"{audit_ok}/{audit_n}" if audit_n > 0 else "-"
        crashed = f" ({r['crashed']} crash)" if r['crashed'] > 0 else ""
        print(f"{aname:44s} {r['total_families']:5d} {r['verified']:5d} {r['passed']:5d} {r['failed']+r['crashed']:5d}{crashed} "
              f"{pre_s:>4s} {cgn_s:>4s} {fact_s:>5s} {audit_s:>5s}")
    print("-" * len(header))
    print(f"{'TOTAL':44s} {total_fams:5d} {total_vrfy:5d} {total_pass:5d} {total_fail:5d}")

    # Print per-family detail rows
    if verify_results:
        print(f"\n  Per-family detail:")
        detail_header = f"{'Family':48s} {'Overall':>8s} {'Pre':>5s} {'Codegen':>8s} {'Fact':>5s} {'Audit':>5s} {'Bench':>6s} {'HU':>4s}"
        print(f"  {detail_header}")
        print(f"  {'-'*len(detail_header)}")
        for (aname, slug), report in sorted(verify_results.items()):
            label = f"{aname}/{slug}"
            overall = report.get("overall_status", "?")
            stages = report.get("stages", {})
            pre = stages.get("preflight", {}).get("status", "skip")[:4]
            cgn = stages.get("codegen", {}).get("status", "skip")[:4]
            fact = stages.get("fact", {}).get("status", "skip")[:4]
            audit = stages.get("audit", {}).get("status", "skip")[:4]
            bench = stages.get("benchmark", {}).get("status", "skip")[:4]
            hu = stages.get("hotupdate", {}).get("status", "skip")[:4]
            print(f"  {label:48s} {overall:>8s} {pre:>5s} {cgn:>8s} {fact:>5s} {audit:>5s} {bench:>6s} {hu:>4s}")

    print()
    return total_fail == 0


# ── Step 5: Dashboard refresh ───────────────────────────────────────────

def step_dashboard():
    """Refresh the verification dashboard."""
    _print_header("Step 5: Dashboard Refresh")
    t0 = time.time()
    r = subprocess.run(REFRESH_CMD, cwd=REPO, capture_output=True, text=True)
    elapsed = time.time() - t0
    if r.returncode != 0:
        print(f"  FAILED ({elapsed:.1f}s)")
        print((r.stderr or "")[:500])
        return False
    print(f"  OK ({elapsed:.1f}s)")
    return True


# ── Main ────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="Foundation-DLL unified verification pipeline")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard",
                        help="Verification mode (default: standard)")
    parser.add_argument("--skip", nargs="*", default=[],
                        help="Additional stages to skip: benchmark hotupdate asm_compare microbench post_hotupdate_benchmark")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="Verbose per-family output")
    parser.add_argument("--family", type=str, default=None,
                        help="Single family to verify (e.g. convert-char). If omitted, verifies all active families.")
    parser.add_argument("--assembly", type=str, default=None,
                        help="Assembly name (default: System.Private.CoreLib). Only used with --family.")
    parser.add_argument("--codegen-mode", type=str, default=None,
                        choices=["aot", "jit"],
                        help="Codegen mode: aot (native C++, default) or jit (interpreter dispatch)")
    args = parser.parse_args()

    errors = []

    # Load ledger
    dlls, lookup = _load_ledger()
    _print_sep()
    print(f"  Foundation-DLL Unified Verification Pipeline")
    print(f"  Ledger: {LEDGER.relative_to(REPO)}")
    print(f"  DLLs: {len(dlls)}, Families: {sum(len(d.get('families', [])) for d in dlls)}")
    print(f"  Mode: {args.mode}, Extra skips: {args.skip or 'none'}")
    _print_sep()
    print()

    # Step 1: Ledger overview
    step_ledger_overview(dlls, lookup)

    # Step 2: Phase 2 generation
    step_phase2()

    # Step 3: Per-family verification
    if args.family:
        # Single-family mode
        assembly = args.assembly or "System.Private.CoreLib"
        families = [(assembly, args.family)]
    else:
        families = _get_verifyable_families(dlls, lookup)

    # Build skip list: by default skip the heaviest stages that need full toolchain.
    # benchmark is NOT skipped -- it's a required stage in standard mode (5 ledger gates).
    default_skip = {"hotupdate", "post_hotupdate_benchmark", "asm_compare", "microbench"}
    skip_stages = list(default_skip.union(set(args.skip)))

    verify_results = step_verify_families(families, mode=args.mode,
                                          skip_stages=skip_stages,
                                          verbose=args.verbose,
                                          codegen_mode=args.codegen_mode)

    # Check for failures
    for (aname, slug), report in verify_results.items():
        overall = report.get("overall_status", "unknown")
        if overall != "passed":
            errors.append(f"{aname}/{slug}: {overall}")

    # Step 4: Results summary
    step_ok = step_results_table(dlls, verify_results)
    if not step_ok:
        errors.append("Some families have verification failures")

    # Step 5: Dashboard refresh
    if not step_dashboard():
        errors.append("Dashboard refresh failed")

    # Final result
    _print_sep()
    if errors:
        print(f"  PIPELINE FAILED ({len(errors)} error(s)):")
        for e in errors:
            print(f"    - {e}")
        _print_sep()
        sys.exit(1)
    else:
        print("  PIPELINE PASSED — all families verified successfully.")
        _print_sep()
        sys.exit(0)


if __name__ == "__main__":
    main()
