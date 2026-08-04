"""python adapter: spawns a pipeline/verification script as a subprocess.

The e2e (foundation-dll) scripts are heavyweight orchestrators that may
themselves run many chunk translations. They are treated as a single test
group: exit 0 => pass, non-zero => fail, with stderr captured as the message.
"""
from __future__ import annotations

import subprocess
import time

from .model import CaseResult, SuiteResult
from .dotnet import repo_root


def run(group: dict, timeout: int = 3600, quick: bool = False) -> SuiteResult:
    cwd = repo_root()
    script = group["script"]
    args = group.get("args", [])
    res = SuiteResult(layer="e2e", group=group.get("name", script))

    cmd = ["python", script] + list(args)
    try:
        t0 = time.time()
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, cwd=cwd)
        res.duration_s = time.time() - t0
        out = (p.stdout or "") + "\n" + (p.stderr or "")
        passed = p.returncode == 0
        res.total = 1
        res.passed = 1 if passed else 0
        res.failed = 0 if passed else 1
        res.cases.append(CaseResult(name=script, passed=passed, message=None if passed else out[-1500:], duration_s=res.duration_s))
    except subprocess.TimeoutExpired:
        res.error = "TIMEOUT: " + script
        res.total = 1
        res.failed = 1
        res.cases.append(CaseResult(name=script, passed=False, message="TIMEOUT"))
    return res
