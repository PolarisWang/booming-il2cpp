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
import os
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
    text = path.read_text(encoding="utf-8")
    try:
        import yaml  # type: ignore
        parsed = yaml.safe_load(text)
    except Exception:
        return _fallback_yaml(text)
    # Consistency guard: when the full yaml lib is present, make sure the
    # no-dependency fallback does not silently diverge from it (P3 fix). If the
    # fallback drops any field, we want a loud failure now, not a silent one on
    # a machine without PyYAML.
    fallback = _fallback_yaml(text)
    if _contract_shape(fallback) != _contract_shape(parsed):
        raise RuntimeError(
            "contract fallback parser diverged from yaml.safe_load — update "
            "_fallback_yaml to parse every field the contract currently uses; "
            f"fallback keys={_contract_shape(fallback)} safe_load={_contract_shape(parsed)}"
        )
    return parsed


def _contract_shape(node) -> str:
    """Compact canonical signature of the contract's field structure, used to
    prove the fallback and safe_load agree on every (nested) key and list length.
    Dict keys are sorted so key ORDER never reports a false divergence."""
    if isinstance(node, dict):
        body = ",".join(
            f"{k}:{_contract_shape(node[k])}"
            for k in sorted(node.keys())
        )
        return "{" + body + "}"
    if isinstance(node, list):
        return "[" + ",".join(_contract_shape(v) for v in node) + "]"
    return type(node).__name__


def _parse_scalar(raw: str):
    """Coerce a YAML scalar token to int/bool/str, approximating safe_load.

    Also strips a trailing inline ` # comment` (YAML spec: a comment after a
    space) so values like `ctest_timeout: 3600  # cap` parse to int 3600.
    """
    s = raw.strip()
    # cut an inline comment (space before #), if present
    if " #" in s:
        s = s.split(" #", 1)[0].rstrip()
    s = s.strip("'\"")
    if s.isdigit():
        return int(s)
    if s in ("true", "True"):
        return True
    if s in ("false", "False"):
        return False
    return s


def _parse_inline_list(raw: str):
    """Parse an inline `[a, b, c]` list. Returns list of scalar tokens."""
    s = raw.strip()
    s = s.strip("[]")
    if not s:
        return []
    return [_parse_scalar(t) for t in s.split(',') if t.strip()]


def _fallback_yaml(text: str) -> dict:
    """Indentation-aware minimal YAML parser for our well-formed contract.

    Requires no third-party dependency (so the driver runs anywhere), but unlike
    the prior version it preserves the FULL contract structure — nested group
    blocks and every scalar field (command_project / cmake_* / timeout / script /
    args / slow ...), plus inline `[a, b]` lists — instead of dropping everything
    below `- name:`. `_load_yaml` cross-checks the result against yaml.safe_load
    and raises loudly if any field diverges.
    """
    # 1) build an indentation tree of explicit nodes: (indent, content)
    node_lines = []  # list of (indent:int, key:str|None, is_seq:bool, text:str)
    for raw in text.splitlines():
        line = raw.rstrip()
        if not line.strip() or line.lstrip().startswith("#"):
            continue
        indent = len(line) - len(line.lstrip())
        s = line.strip()
        if s.startswith("- "):
            body = s[2:].strip()
            # "- key: value" or "- bare"
            if ":" in body:
                k, _, v = body.partition(":")
                node_lines.append((indent, k.strip(), True, v.strip()))
            else:
                node_lines.append((indent, None, True, body))
        elif ":" in s:
            k, _, v = s.partition(":")
            node_lines.append((indent, k.strip(), False, v.strip()))
        else:
            node_lines.append((indent, None, False, s))

    def build(start: int, end: int, base_indent: int):
        """Build a dict/list from node_lines[start:end] whose known parents are
        at base_indent. Nodes with indent <= base_indent terminate this block."""
        node = {}
        i = start
        while i < end:
            ind, key, is_seq, val = node_lines[i]
            # find extent of this node's children
            j = i + 1
            while j < end and node_lines[j][0] > ind:
                j += 1
            chunk = node_lines[i + 1:j] if j > i + 1 else []
            if is_seq:
                # a sequence item: - name: X  -> dict with its keys; collect
                # consecutive seq items at same indent into a list
                seq_items = []
                m = i
                while m < end and node_lines[m][0] == ind and node_lines[m][2] is True:
                    _, k2, _, v2 = node_lines[m]
                    item_end = m + 1
                    while item_end < end and node_lines[item_end][0] > ind:
                        item_end += 1
                    child = build(m + 1, item_end, ind) if item_end > m + 1 else {}
                    if key is not None:
                        # inline key on the dash line: "- name: codegen"
                        if v2.startswith("["):
                            child[k2] = _parse_inline_list(v2)
                        elif v2:
                            child[k2] = _parse_scalar(v2)
                        seq_items.append(child)
                    else:
                        seq_items.append(_parse_scalar(val) if v2 else child)
                    m = item_end
                i = m
                # attach the sequence to its parent key (seq items carry no key
                # at the parent level in our contract: they live under groups:)
                node.setdefault("_SEQ_", []).extend(seq_items)
            else:
                if val:
                    # scalar or inline list
                    node[key] = _parse_inline_list(val) if val.startswith("[") else _parse_scalar(val)
                else:
                    # nested block: build from chunk
                    node[key] = build(i + 1, j, ind) if chunk else {}
                i = j
        return node

    root = build(0, len(node_lines), -1)

    # 2) promote the parent-key `_SEQ_` markers into real "groups" lists
    def promote(node):
        if isinstance(node, list):
            return [promote(v) for v in node]
        if not isinstance(node, dict):
            return node
        out = {}
        had_seq = False
        for k, v in node.items():
            if k == "_SEQ_":
                had_seq = True
                out["groups"] = [promote(x) for x in v]
            else:
                out[k] = promote(v)
        # a node whose entire value is a sequence (e.g. the `groups:` block)
        # flattens to just the list, otherwise we'd get {"groups":{"groups":[...]}}
        if had_seq and len(out) == 1 and "groups" in out:
            return out["groups"]
        return out

    return promote(root)


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
    # a group may override the layer's adapter; else use layer default
    adapter_name = group.get("adapter") or layers_cfg.get(layer, {}).get("adapter", "dotnet")
    run = ADAPTERS.get(adapter_name, dotnet.run)
    # per-group timeout override (heavyweight e2e pipelines run longer than
    # the driver default); falls back to the global --timeout
    timeout = group.get("timeout", opts.timeout)
    return run(group, timeout=timeout, quick=opts.quick)


JSON_SCHEMA = """\
Stable machine contract for --json / --machine output (version: 1)

Top level:
  version        int     = 1
  generated_at   str     ISO-8601 timestamp
  ok             bool    True iff every group had no infra error AND no UNEXPECTED failure
  duration_s     float   wall-clock seconds for the whole run
  total          object  { passed: int, failed: int, total: int }
  known          int     number of failing cases annotated as KNOWN-FAIL (baseline)
  layers         object  keyed by layer name ("unit"|"integration"|"e2e")
                         value: { adapter: str, groups: <groups> }

groups (layers.<layer>.groups.<group>):
  ok             bool    True iff ran without infra error AND unexpected is empty
  passed         int
  failed         int
  total          int
  error          str|null
  duration_s     float
  known          int     failing cases matched by baselines/known-failures.<layer>.yaml
  unexpected     [str]   NEW failures (not in the known baseline) — an empty list is required
                         for the gate to be green
  stale_known    [str]   names the baseline lists as known-fail but which PASSED this run
                         (baseline should be pruned)
  ghost_known    [str]   names the baseline lists as known-fail but which NEVER ran this run
                         (test removed/renamed → ghost entry; baseline should be pruned)
  failures       [{ name, msg }]   every failing case (known or not), msg truncated to 500
  cases           — only present with --cases: [{ name, passed, duration_s, msg? }]

Gate: report.ok == False  <=>  exit code 1.  This happens iff some group's
`unexpected` is non-empty (or it infra-errored). KNOWN-FAIL cases are reported under
`failures` but do NOT flip ok→False.
"""


_HELP_EPILOG = """\
Layers (test pyramid, defined in tests/suite_contract.yaml):
  unit         fast dotnet xunit  -> tests/unit/managed/{codegen,driver,snapshot}
  integration  native CTest        -> tests/unit/runtime-native (needs cmake --preset first)
  e2e          foundation-dll      -> tests/e2e/verification engine (slow, heavy native builds)

Examples:
  python tests/runner/test_driver.py --layer unit --quick      # fast dotnet suite (prebuilt)
  python tests/runner/test_driver.py --dry-run                 # preview every layer/group, run nothing
  python tests/runner/test_driver.py --layer e2e --group foundation-dll-chunk  # one e2e group
  python tests/runner/test_driver.py --layer all --json out.json   # everything, machine report

Exit code:
  0 = OVERALL OK (every group ran; no UNEXPECTED failure — KNOWN-FAIL baseline failures are
      reported as FAIL but do NOT fail the gate)
  1 = FAILED (an infra error or at least one UNEXPECTED failure not in the known baseline)

Agents: consume --json (or --machine for a fixed path) — parse the JSON, not stdout.
  `python tests/runner/test_driver.py --contract` prints the exact JSON schema (stable
  machine contract). Use --dry-run to inspect the suite without running it.
Known-failure reconciliation: baselines/known-failures.{layer}.yaml.
"""


def _dry_run(args, contract: dict, layers: dict) -> int:
    """Preview what the suite would run (per layer/group) — execute nothing."""
    print("=== Dry run: test-driver would run the following ===")
    for layer_name, layer_cfg in layers.items():
        if args.layer != "all" and layer_name != args.layer:
            continue
        adapter = layer_cfg.get("adapter")
        print(f"\n[{layer_name}] adapter={adapter}")
        for g in layer_cfg.get("groups", []):
            gname = g.get("name", "")
            if args.group and gname != args.group:
                continue
            if adapter == "dotnet":
                target = g.get("command_project", g.get("script", "?"))
                scale = "dotnet test project"
            elif adapter == "native":
                target = g.get("cmake_build_dir", "build/native")
                if os.environ.get("CHAOS_GC_TEST_STRESS_ONLY", "") in ("1", "true", "yes"):
                    scale = "ctest -L 'stress|soak|benchmark'  (--stress-only pressure tier)"
                else:
                    scale = f"ctest -LE {g.get('ctest_exclude', 'benchmark|stress|soak')}"
            else:  # python
                target = g.get("script", "?")
                scale = f"{' '.join(g.get('args', [])) or '<no args>'}"
            timeout = g.get("timeout", args.timeout)
            print(f"  - {gname}: {target}")
            print(f"      {scale}   (timeout {timeout}s)")
    print("\n(Dry run only — nothing executed.)")
    return 0


def main() -> int:
    # Load the contract first so --layer choices and --help reflect new layers in
    # suite_contract.yaml (e.g. a pytest layer). On parse error still allow --help
    # and --contract to work.
    try:
        contract = load_contract()
        layer_choices = list(contract.get("layers", {}).keys()) + ["all"]
    except Exception:
        contract = None
        layer_choices = ["unit", "integration", "e2e", "pytest", "all"]
    ap = argparse.ArgumentParser(
        description="Chaos IL2CPP unified test driver — runs the suite defined by "
                    "tests/suite_contract.yaml (single source of truth).",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=_HELP_EPILOG,
    )
    ap.add_argument("--layer", default="all", choices=layer_choices,
                    help="which pyramid layer(s) to run (default: all). "
                         "unit=fast dotnet (tests/unit/managed/*); "
                         "integration=native CTest (tests/unit/runtime-native, needs cmake built); "
                         "e2e=foundation-dll engine (tests/e2e/verification, slow); "
                         "pytest=authority/integration pytest subset.")
    ap.add_argument("--group", default="",
                    help="run only the named group within the selected layer(s) "
                         "(e.g. --layer unit --group codegen)")
    ap.add_argument("--quick", action="store_true",
                    help="assume prebuilt; skip configure")
    ap.add_argument("--stress-only", action="store_true",
                    help="run ONLY the pressure/benchmark/soak tests (independent tier) "
                         "instead of the fast unit set — sets CHAOS_GC_TEST_STRESS_ONLY; "
                         "the native adapter then uses ctest -L 'stress|soak|benchmark')")
    ap.add_argument("--timeout", type=int, default=1800,
                    help="per-group timeout in seconds (default 1800; integration suite "
                         "also honors group ctest_timeout in suite_contract)")
    ap.add_argument("--dry-run", action="store_true",
                    help="do NOT run anything — print what each layer/group would run "
                         "(adapter, target, group timeout) and exit 0. Use this to preview "
                         "or to answer 'what does the suite contain?'")
    ap.add_argument("--json", default="",
                    help="write a stable machine-readable report (default "
                         "tests/runner/test-report.json); agents should parse --json, not stdout")
    ap.add_argument("--machine", action="store_true",
                    help="same as --json but always to the fixed contract path "
                         "tests/runner/test-report.json (a stable location agents can rely on)")
    ap.add_argument("--contract", action="store_true",
                    help="print the stable --json report schema (machine contract) and exit — "
                         "use this to fetch the exact JSON shape an agent should parse")
    ap.add_argument("--junit", default="",
                    help="also write a JUnit XML report to this path (CI integration)")
    ap.add_argument("--cases", action="store_true",
                    help="include per-case pass/fail detail in the JSON report "
                         "(default off: native ~200 cases, dotnet can be thousands)")
    args = ap.parse_args()

    # P1-2: --stress-only flips the native adapter to run the independent
    # pressure tier via ctest -L 'stress|soak|benchmark' instead of -LE exclude.
    if getattr(args, "stress_only", False):
        os.environ["CHAOS_GC_TEST_STRESS_ONLY"] = "1"

    if args.contract:
        print(JSON_SCHEMA)
        return 0

    if contract is None:
        contract = load_contract()
    layers = contract.get("layers", {})
    if not layers:
        raise SystemExit("contract has no layers — check suite_contract.yaml")

    if args.dry_run:
        return _dry_run(args, contract, layers)

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
            # Count only FAILED cases that are baseline-known. Previously this
            # counted every named-known case regardless of pass/fail, which
            # inflated "known" with stale (already-recovered) entries and made
            # `known` disagree with the number of actual known failures.
            known_in_run = [c for c in res.cases if not c.passed and c.name in known]
            known_found += len(known_in_run)

            # we don't drop known failures; we annotate them
            for c in res.cases:
                if c.name in known:
                    c.message = ("[KNOWN-FAIL] " + (c.message or "")).strip()

            # P4: explicit regression gate. A group is OK if the adapter ran
            # without an infra error (error is None, total > 0) AND every failure
            # is baseline-known. `unexpected` is the set of NEW failures (not in
            # the known baseline) — the thing that must flip the gate red.
            # NOTE: we deliberately do NOT use SuiteResult.ok here, because it
            # already returns False when ANY failure exists (known or not), which
            # would defeat the known-failure reconciliation (baseline-known
            # failures are expected and must not turn the gate red).
            ran_ok = res.error is None and res.total > 0
            unexpected = [c for c in res.cases if not c.passed and c.name not in known]
            group_ok = ran_ok and not unexpected

            # P4: stale-known diagnostic — baseline lists a name as known-fail
            # but it PASSED this run. That entry is now doing nothing useful
            # (the test has recovered); surface it so the baseline can be pruned
            # and the gate re-armed for that name.
            passed_names = {c.name for c in res.cases if c.passed}
            stale_known = sorted(known & passed_names)

            # P4b: ghost-known diagnostic — a known-fail name that NEVER appeared in
            # this run at all (neither passed nor failed).  stale_known only fires when
            # a known test passes; if the underlying test was removed/renamed, the
            # known-fail entry silently stops matching and becomes a ghost.  Surface
            # it so the baseline can be pruned rather than decaying silently.
            run_names = {c.name for c in res.cases}
            ghost_known = sorted(known - run_names)

            grand_total += res.total
            grand_pass += res.passed
            grand_fail += res.failed

            group_entry = {
                "ok": group_ok, "passed": res.passed, "failed": res.failed,
                "total": res.total, "error": res.error, "duration_s": res.duration_s,
                "known": len(known_in_run),
                "unexpected": [c.name for c in unexpected],
                "stale_known": stale_known,
                "ghost_known": ghost_known,
                "failures": [{"name": c.name, "msg": (c.message or "")[:500]}
                             for c in res.cases if not c.passed],
            }
            # Relay the adapter's captured raw tool stdout to THIS process's stdout.
            # The native adapter captures ctest output instead of streaming it, so
            # the nightly `test_driver --stress-only | tee gc-stress-output.txt`
            # would otherwise never see the benchmark `BENCH,<name>,KEY=VAL` lines
            # the GC/JIT collector parses.  Emit them here so the tee + downstream
            # collect-gc-metrics.py / collect-jit-metrics.py get real data.
            if getattr(res, "raw_out", ""):
                sys.stdout.write(getattr(res, "raw_out", ""))
                sys.stdout.flush()
            # P5: optional per-case detail (off by default — native ~200 cases,
            # dotnet can be thousands). With --cases this lets a later run diff
            # exactly which tests passed/failed for regression triage. --junit also
            # needs the full case list to emit one <testcase> per test, so a JUnit
            # consumer isn't handed an inconsistent testsuite (tests=N but only
            # failure testcases present).
            if args.cases or args.junit:
                group_entry["cases"] = [
                    {"name": c.name, "passed": c.passed, "duration_s": round(c.duration_s, 3),
                     **({"msg": (c.message or "")[:500]} if c.message else {})}
                    for c in res.cases
                ]
            report["layers"][layer_name]["groups"][gname] = group_entry
            if not group_ok:
                overall_ok = False

    report["total"] = {"passed": grand_pass, "failed": grand_fail, "total": grand_total}
    report["ok"] = overall_ok
    report["known"] = known_found
    report["duration_s"] = round(time.time() - t0, 2)

    # --machine forces the fixed contract path so agents can rely on one location.
    json_path = args.json or (str(ROOT / "tests" / "runner" / "test-report.json")
                              if args.machine else str(ROOT / "tests" / "runner" / "test-report.json"))
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
            for u in g.get("unexpected", []):
                print(f"        UNEXPECTED-FAIL {u}")
            for s in g.get("stale_known", []):
                print(f"        STALE-KNOWN (now passing, prune baseline): {s}")
            for s in g.get("ghost_known", []):
                print(f"        GHOST-KNOWN (known-fail test never ran — removed/renamed? prune baseline): {s}")
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
            # Emit one <testcase> per test. Passing cases carry no <failure> child;
            # failing/skipped-folded cases carry one. --junit implies full case detail
            # (else the suite would declare tests=N but emit only the failure testcases,
            # under-reporting passes to any JUnit consumer).
            cases = g.get("cases")
            if cases:
                for c in cases:
                    tc = ET.SubElement(ts, "testcase", {"name": c["name"], "classname": gname})
                    if not c["passed"]:
                        ET.SubElement(tc, "failure", {"message": c.get("msg", "")[:500]})
            else:
                # Fallback when no per-case detail (unexpected): emit failures only.
                for f in g["failures"]:
                    tc = ET.SubElement(ts, "testcase", {"name": f["name"], "classname": gname})
                    ET.SubElement(tc, "failure", {"message": f["msg"]})
    ET.ElementTree(root).write(path, encoding="utf-8")


def _now() -> str:
    import datetime
    return datetime.datetime.now().isoformat(timespec="seconds")


if __name__ == "__main__":
    sys.exit(main())
