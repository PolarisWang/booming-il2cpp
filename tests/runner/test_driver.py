#!/usr/bin/env python3
"""test_driver — single entry point for the entire Chaos IL2CPP test suite.

Reads tests/suite_contract.yaml, dispatches each layer+group to its adapter
(dotnet / native / python), reconciles results against known-failure baselines
(which label failures "known" WITHOUT skipping them — they still FAIL), and
writes a unified JSON report.

Usage:
    python tests/runner/test_driver.py [--layer unit|integration|e2e|all]
        [--group NAME] [--quick] [--timeout N] [--json FILE] [--junit FILE]

Exit code: 0 if every group is OK (accounting for known failures), else 1.
No "skip" concept: every test PASSES or FAILS; known failures are a FAIL
that the baseline labels as expected, reported transparently.

CLAUDE.md layer-boundary note: this driver only *runs* tests; it never writes
codegen (.cpp/.h) or generated artifacts. Python adapter only spawns existing
pipeline scripts.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

from adapters import dotnet, native, python
from adapters.model import SuiteResult

ROOT = Path(__file__).resolve().parents[2]  # <repo>/tests/runner -> <repo>
CONTRACT_PATH = ROOT / "tests" / "suite_contract.yaml"
KNOWN_FAIL_DIR = ROOT / "tests" / "runner" / "baselines"

ADAPTERS = {"dotnet": dotnet.run, "native": native.run, "python": python.run}


def load_contract() -> dict:
    if not CONTRACT_PATH.exists():
        raise SystemExit(f"contract not found: {CONTRACT_PATH}")
    return _load_yaml(CONTRACT_PATH)


def _load_yaml(path: Path) -> dict:
    # tiny YAML subset (layers -> groups). No third-party dependency so the
    # driver runs anywhere. Only the structure we emit is supported.
    try:
        import yaml  # type: ignore
        return yaml.safe_load(path.read_text(encoding="utf-8"))
    except Exception:
        return _fallback_yaml(path.read_text(encoding="utf-8"))


def _fallback_yaml(text: str) -> dict:
    # minimal parser for our well-formed contract; avoids hard yaml dep
    layers: dict = {}
    cur_layer = None
    for raw in text.splitlines():
        line = raw.rstrip()
        if not line or line.lstrip().startswith("#"):
            continue
        indent = len(line) - len(line.lstrip())
        s = line.strip()
        if indent == 0 and s.endswith(":") and "  " not in s:
            cur_layer = s[:-1]
            layers[cur_layer] = {"adapter": None, "groups": []}
        elif cur_layer and s.startswith("adapter:"):
            layers[cur_layer]["adapter"] = s.split(":", 1)[1].strip()
        elif cur_layer and s.startswith("name:") and layers[cur_layer].get("_g"):
            pass
        elif cur_layer and s.startswith("- name:"):
            layers[cur_layer]["groups"].append({"name": s.split(":", 1)[1].strip()})
    return {"layers": layers, "version": 1}


def load_known_failures(layer: str) -> set:
    """Return names of tests known to fail for this layer (from baselines/)."""
    for ext in ("yaml", "json"):
        p = KNOWN_FAIL_DIR / f"known-failures.{layer}.{ext}"
        if p.exists():
            try:
                data = _load_yaml(p)
                fams = data.get("known", []) if isinstance(data, dict) else data
                return set(fams)
            except Exception:
                continue
    return set()


def run_group(layer: str, group: dict, layers_cfg: dict, opts) -> SuiteResult:
    adapter_name = layers_cfg.get(layer, {}).get("adapter", "dotnet")
    run = ADAPTERS.get(adapter_name, dotnet.run)
    return run(group, timeout=opts.timeout, quick=opts.quick)


def main() -> int:
    ap = argparse.ArgumentParser(description="Chaos IL2CPP unified test driver")
    ap.add_argument("--layer", default="all", choices=["unit", "integration", "e2e", "all"])
    ap.add_argument("--group", default="")
    ap.add_argument("--quick", action="store_true", help="assume prebuilt; skip configure")
    ap.add_argument("--timeout", type=int, default=1800)
    ap.add_argument("--json", default="")
    ap.add_argument("--junit", default="")
    args = ap.parse_args()

    contract = load_contract()
    layers = contract.get("layers", {})
    if not layers:
        raise SystemExit("contract has no layers — check suite_contract.yaml")
    t0 = time.time()

    report = {"version": 1, "generated_at": _now(), "layers": {}, "total": {}, "ok": True}
    overall_ok = True
    grand_total = grand_pass = grand_fail = 0
    known_found = 0

    for layer_name, layer_cfg in layers.items():
        if args.layer != "all" and layer_name != args.layer:
            continue
        report["layers"][layer_name] = {"adapter": layer_cfg.get("adapter"), "groups": {}}
        for g in layer_cfg.get("groups", []):
            gname = g.get("name", "")
            if args.group and gname != args.group:
                continue
            try:
                res = run_group(layer_name, g, layers, args)
            except Exception as e:  # any adapter crash => infra failure, not skip
                res = SuiteResult(layer=layer_name, group=gname, error=str(e))

            # reconcile known failures
            known = load_known_failures(layer_name)
            known_in_run = [c for c in res.cases if c.name in known]
            known_found += len(known_in_run)

            kf = {}
            # we don't drop known failures; we annotate them
            for c in res.cases:
                if c.name in known:
                    c.message = ("[KNOWN-FAIL] " + (c.message or "")).strip()

            group_ok = res.ok
            # if every failure is a known one, the group counts as OK for gating
            if res.failed and known_in_run and len(known_in_run) == res.failed:
                group_ok = True

            grand_total += res.total
            grand_pass += res.passed
            grand_fail += res.failed

            report["layers"][layer_name]["groups"][gname] = {
                "ok": group_ok, "passed": res.passed, "failed": res.failed,
                "total": res.total, "error": res.error, "duration_s": res.duration_s,
                "known": len(known_in_run),
                "failures": [{"name": c.name, "msg": (c.message or "")[:500]}
                             for c in res.cases if not c.passed],
            }
            if not group_ok:
                overall_ok = False

    report["total"] = {"passed": grand_pass, "failed": grand_fail, "total": grand_total}
    report["ok"] = overall_ok
    report["known"] = known_found
    report["duration_s"] = round(time.time() - t0, 2)

    json_path = args.json or str(ROOT / "tests" / "runner" / "test-report.json")
    Path(json_path).write_text(json.dumps(report, indent=2), encoding="utf-8")
    if args.junit:
        _write_junit(report, args.junit)

    _print_human(report)
    return 0 if overall_ok else 1


def _print_human(report: dict) -> None:
    print("\n=== Chaos IL2CPP Test Report ===")
    for layer, lc in report["layers"].items():
        for gname, g in lc["groups"].items():
            flag = "PASS" if g["ok"] else "FAIL"
            print(f"  [{flag}] {layer}/{gname}: pass={g['passed']} fail={g['failed']} known={g['known']} {g['duration_s']}s")
            if g["error"]:
                print(f"        infra error: {g['error'][:300]}")
            for f in g["failures"]:
                print(f"        FAIL {f['name']}\n             {f['msg'][:200]}")
    t = report["total"]
    print(f"  TOTAL: pass={t['passed']} fail={t['failed']} total={t['total']} known={report['known']}")
    print(f"  OVERALL: {'OK' if report['ok'] else 'FAILED'} ({report['duration_s']}s)")


def _write_junit(report: dict, path: str) -> None:
    import xml.etree.ElementTree as ET
    root = ET.Element("testsuites", {"name": "chaos-il2cpp", "tests": str(report["total"]["total"])})
    for layer, lc in report["layers"].items():
        for gname, g in lc["groups"].items():
            ts = ET.SubElement(root, "testsuite", {
                "name": f"{layer}.{gname}", "tests": str(g["total"]),
                "failures": str(g["failed"]), "errors": "0" if not g["error"] else "1",
            })
            for f in g["failures"]:
                tc = ET.SubElement(ts, "testcase", {"name": f["name"], "classname": gname})
                ET.SubElement(tc, "failure", {"message": f["msg"]})
    ET.ElementTree(root).write(path, encoding="utf-8")


def _now() -> str:
    import datetime
    return datetime.datetime.now().isoformat(timespec="seconds")


if __name__ == "__main__":
    sys.exit(main())
