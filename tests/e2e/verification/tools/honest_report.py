"""Honest per-chunk fact+benchmark+hotupdate report — shows real-vs-smoke, gate status.

Usage:
    python -m verification.tools.honest_report [--gate-ratio 0.10]

Outputs a tabular snapshot identical to the per-chunk inspection format used
during the 2026-09-09 review — each chunk one row, fact columns split into
nominal vs real (smoke-42 subtracted), benchmark excluded from counts when
the chunk's fact-gate status is 'fail'.

Also writes to nightly-build-report/ for posterity.
"""

from __future__ import annotations

import json, os, sys, argparse, time
from pathlib import Path

# Bootstrap: find the verification package parent (tests/e2e/) so standalone
# `python -m verification.tools.honest_report` works from any cwd.
_HERE = Path(__file__).resolve().parent  # verification/tools/
_VERIFY = _HERE.parent                   # verification/
_E2E = _VERIFY.parent                    # tests/e2e/
if str(_E2E) not in sys.path:
    sys.path.insert(0, str(_E2E))

# Failure-tolerant import of gating; if verification.stages fails because of
# massive import cascade (build.py -> _pipeline), load the module directly.
try:
    from verification.stages.gating import classify_gate, GATE_REAL_RATIO
except Exception:
    import importlib.util
    _gating_path = _VERIFY / "stages" / "gating.py"
    if _gating_path.exists():
        _spec = importlib.util.spec_from_file_location("_gate_import", str(_gating_path))
        _mod = importlib.util.module_from_spec(_spec)
        _spec.loader.exec_module(_mod)
        classify_gate = _mod.classify_gate
        GATE_REAL_RATIO = _mod.GATE_REAL_RATIO
    else:
        raise


def _fact_stats(fr: Path):
    """Read per-chunk fact-results and derive real-vs-smoke from the
    runtime-stamped ``resultKind`` field (written by fact_chunk.py _annotate).

    Factories to ``fact.json`` for legacy chunks (pre-runtime-enrichment) which
    use ``realTotal``/``realPassed`` / fall back to marker-based counting.

    The returned dict always has ``{total, nominal, realVerified, unassertable,
    smokeUnknown, failed}`` so consumers never see missing keys.
    """
    result = dict(total=0, nominal=0, realVerified=0, unassertable=0,
                  smokeUnknown=0, failed=0)
    if not fr.is_file():
        return None
    d = json.loads(fr.read_text(encoding="utf-8"))
    a = d.get("aot", [])
    result["total"] = len(a)
    if result["total"] == 0:
        return None
    # Check for the runtime-enriched resultKind (the single source of truth)
    kinds = [r.get("resultKind") for r in a if r.get("resultKind")]
    if kinds:
        result["nominal"] = sum(1 for r in a if r.get("passed"))
        result["realVerified"] = sum(1 for k in kinds if k == "real")
        result["unassertable"] = sum(1 for k in kinds if k == "unassertable")
        result["smokeUnknown"] = sum(1 for k in kinds if k == "smoke")
        result["failed"] = sum(1 for k in kinds if k == "failed")
        return result

    # Legacy: no resultKind — fall back to value==42 detection
    result["nominal"] = sum(1 for r in a if r.get("passed"))
    sm = sum(1 for r in a if r.get("value") == 42)
    t = result["total"]
    result["realVerified"] = max(0, result["nominal"] - sm)
    result["unassertable"] = 0
    result["smokeUnknown"] = sm
    result["failed"] = t - result["nominal"]
    return result


def _bench_stats(br: Path):
    if not br.is_file(): return None
    d = json.loads(br.read_text(encoding="utf-8"))
    if isinstance(d, dict):
        return dict(methods=d.get("methodCount", 0), stub=d.get("stubCount", 0),
                    nonStub=d.get("nonStubCount", 0))
    elif isinstance(d, list):
        st = sum(1 for m in d if isinstance(m, dict) and (m.get("meanDurationMs", 0) or 0) <= 0.001)
        return dict(methods=len(d), stub=st, nonStub=len(d) - st)
    return None


def _hu_stats(hr: Path):
    if not hr.is_file(): return None
    d = json.loads(hr.read_text(encoding="utf-8"))
    if not isinstance(d, dict): return None
    return dict(semantic=d.get("semanticChangedCount", 0),
                patch=d.get("activePatchCount", 0),
                revert=d.get("totalReverted", d.get("revertPassed", 0)))


def main() -> int:
    p = argparse.ArgumentParser(description="Honest per-chunk snapshot")
    p.add_argument("--gate-ratio", type=float, default=None,
                   help="Override GATE_REAL_RATIO (default: env CHAOS_GATE_REAL_RATIO or 0.10)")
    p.add_argument("--root-fact", default="translation",
                   help="Root directory for fact-results.json (default: translation)")
    p.add_argument("--root-bench", default="artifacts/foundation-dll",
                   help="Root directory for benchmark/hotupdate data (default: artifacts/foundation-dll)")
    args = p.parse_args()

    gate_ratio = args.gate_ratio if args.gate_ratio is not None else GATE_REAL_RATIO
    root_fact = Path(args.root_fact)
    root_bench = Path(args.root_bench)

    rows: list[dict] = []
    if not root_fact.is_dir() and not root_fact.is_symlink():
        root_fact = _E2E / root_fact
    if not root_bench.is_dir() and not root_bench.is_symlink():
        root_bench = _E2E / root_bench

    # collect keys from fact tree
    seen = {}
    for base in [root_fact]:
        if not base.is_dir(): continue
        for asm in sorted(os.listdir(base)):
            cd = base / asm / "chunks"
            if not cd.is_dir(): continue
            for slug in sorted(os.listdir(cd)):
                if slug.startswith(".hep"): continue
                key = f"{asm}/{slug}"
                seen[key] = {"asm": asm, "slug": slug}

    # fact
    for key, info in seen.items():
        fr = root_fact / info["asm"] / "chunks" / info["slug"] / "results" / "fact-results.json"
        f = _fact_stats(fr)
        if f:
            info["fact"] = f
    # benchmark + hotupdate from artifacts build-output tree
    for base in [root_bench]:
        if not base.is_dir(): continue
        for asm in sorted(os.listdir(base)):
            cd = base / asm / "chunks"
            if not cd.is_dir(): continue
            for slug in sorted(os.listdir(cd)):
                if slug.startswith(".hep"): continue
                key = f"{asm}/{slug}"
                if key not in seen: seen[key] = {"asm": asm, "slug": slug}
                info = seen[key]
                res = cd / slug / "results"
                b = _bench_stats(res / "benchmark.json")
                if b: info["bench"] = b
                h = _hu_stats(res / "hotupdate.json")
                if h: info["hu"] = h

    # build table
    lines = []
    hdr = (f"{'CHUNK':56s}  {'F_tot':>5}  {'F_nom':>4}"
           f"  {'real':>4}  {'unassert':>7}  {'smoke?':>6}  {'fail':>4}"
           f"  {'real%':>5}   {'B_ok':>4}{'B_stub':>5}   {'HU_sem':>6}{'HU_pch':>6}   {'GATE':>4}")
    sep = "-" * len(hdr)
    lines.append("")
    lines.append(hdr)
    lines.append(sep)

    ft, fn, frl, fu, fs, ff, bo, bs, hs, hp, bg, hg = [0] * 12
    gk = []
    for key in sorted(seen, key=lambda k: k.lower()):
        info = seen[key]
        f = info.get("fact")
        b = info.get("bench")
        h = info.get("hu")
        if not f and not b and not h:
            continue

        # compute gate
        gt = classify_gate(f) if f else "skip"
        real_v = f["realVerified"] if f else 0
        unassert = f["unassertable"] if f else 0
        smoke_u = f["smokeUnknown"] if f else 0
        fail_ct = f["failed"] if f else 0
        pct = real_v / f["total"] * 100 if f and f["total"] > 0 else 0.0

        b_stub = b["stub"] if b else 0
        b_total = (b["nonStub"] or 0) if b else 0
        h_sem = h["semantic"] if h else 0
        h_pch = h["patch"] if h else 0
        if f:
            ft += f["total"]; fn += f["nominal"]
            frl += real_v; fu += unassert; fs += smoke_u; ff += fail_ct
        if b: bo += b["nonStub"]; bs += b["stub"]
        if h: hs += h["semantic"]; hp += h["patch"]
        if gt == "fail": bg += 1; gk.append(key); hg += 1
        gs = "PASS" if gt == "pass" else "FAIL" if gt == "fail" else "skip"
        lines.append(f"{key:56s}  {f['total'] if f else 0:5d}  {f['nominal'] if f else 0:4d}"
                     f"  {real_v:4d}  {unassert:7d}  {smoke_u:6d}  {fail_ct:4d}"
                     f"  {pct:4.1f}%   {b_total:4d}{b_stub:5d}   {h_sem:6d}{h_pch:6d}   {gs:>4}")

    lines.append(sep)
    lines.append(f"{'TOTAL':56s}{ft:5d}{fn:6d}{frl:5d}{fu:8d}{fs:6d}{ff:<5}       {bo:5d}{bs:5d}  {hs:6d}{hp:6d}  gated_b={bg}  gated_hu={hg}")
    lines.append(f"\nGate ratio threshold: {gate_ratio}  |  Gated chunks: {bg}")
    if gk:
        lines.append(f"  gated list: {', '.join(gk)}")

    out = "\n".join(lines)
    print(out)

    # persist
    report_root = root_fact.parent / "nightly-build-report" if (root_fact.parent / "nightly-build-report").is_dir() else _E2E / "nightly-build-report"
    report_root.mkdir(parents=True, exist_ok=True)
    stamp = time.strftime("%Y%m%d_%H%M%S")
    out_path = report_root / f"honest-report-{stamp}.txt"
    out_path.write_text(out, encoding="utf-8")
    print(f"\n(written to {out_path})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())