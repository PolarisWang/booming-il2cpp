"""dotnet adapter: runs `dotnet test` for a project and parses the outcome.

Runs with default (minimal) verbosity so the VSTest summary line is emitted
("Passed! - Failed: 0, Passed: N, Skipped: N, Total: N ..."), plus a trx logger
so exact per-test failure names can be extracted without relying on console
detail verbosity (which drops the summary line).

No 'skip' is reported: the Skipped count is folded into FAILURES (project rule
disallows skip); a skipped test is surfaced as a failure, never hidden.
"""
from __future__ import annotations

import os
import re
import subprocess
import tempfile
import time
import xml.etree.ElementTree as ET
from pathlib import Path

from .model import CaseResult, SuiteResult


def repo_root() -> str:
    # this file lives at <root>/tests/runner/adapters/ -> root is 3 up
    return str(Path(__file__).resolve().parents[3])


def _parse_summary(out: str) -> dict:
    m = re.search(
        r"Failed:\s*(\d+),\s*Passed:\s*(\d+),\s*Skipped:\s*(\d+),\s*Total:\s*(\d+)",
        out, re.IGNORECASE,
    )
    if not m:
        return {}
    return {
        "failed": int(m.group(1)),
        "passed": int(m.group(2)),
        "skipped": int(m.group(3)),
        "total": int(m.group(4)),
    }


def _trx_failures(trx_path: str) -> list[str]:
    try:
        tree = ET.parse(trx_path)
        ns = "http://microsoft.com/schemas/VisualStudio/TeamTest/2010"
        out = []
        for u in tree.getroot().iter("{%s}UnitTestResult" % ns):
            # Project rule: no skip. A Skipped outcome is folded into failures so it
            # is surfaced (never hidden) and reaches `unexpected` in the gate — matching
            # res.failed = failed + skipped in run().
            if u.get("outcome") in ("Failed", "Skipped"):
                out.append(u.get("testName"))
        return out
    except Exception:
        return []


def run(group: dict, timeout: int = 900, quick: bool = False) -> SuiteResult:
    project = group["command_project"]
    res = SuiteResult(layer="unit", group=group.get("name", project))

    with tempfile.TemporaryDirectory(prefix="chaos-driver-") as td:
        trx = os.path.join(td, "result.trx")
        # default verbosity emits the summary; trx gives exact failure names
        args = ["dotnet", "test", project, "--no-restore",
                "--logger", f"trx;LogFileName={trx}"]
        if quick:
            args += ["--no-build"]
        t0 = time.time()
        try:
            p = subprocess.run(args, capture_output=True, text=True,
                               timeout=timeout, cwd=repo_root())
            out = (p.stdout or "") + "\n" + (p.stderr or "")
            rc = p.returncode
        except subprocess.TimeoutExpired:
            res.error = "TIMEOUT: " + project
            res.duration_s = time.time() - t0
            return res
        res.duration_s = time.time() - t0

        if rc == 124:
            res.error = "TIMEOUT: " + project
            return res

        if not os.path.exists(trx):
            # build failure or no test adapter — surface raw output
            res.error = "No trx produced (build failure / no tests?):\n" + out[-1500:]
            return res

        summary = _parse_summary(out)
        res.total = summary.get("total", 0)
        res.passed = summary.get("passed", 0)
        res.failed = summary.get("failed", 0) + summary.get("skipped", 0)

        failures = _trx_failures(trx)
        res.cases = [CaseResult(name=n, passed=False) for n in failures]
        if not res.cases and res.failed:
            res.cases = [CaseResult(name="<rollup>", passed=False)]
        if res.total == 0:
            res.error = "trx present but total=0 (parse issue?):\n" + out[-1000:]
    return res
