"""python adapter: spawns a pipeline/verification script as a subprocess.

The e2e (foundation-dll) scripts are heavyweight orchestrators that may
themselves run many chunk translations. They are treated as a single test
group: exit 0 => pass, non-zero => fail, with stderr captured as the message.

The relocated engine (tests/e2e/verification) finds its family tree
(tests/e2e/translation) via env overrides (CHAOS_FOUNDATION_DLL /
CHAOS_TESTING_DIR) + PYTHONPATH. A group may declare these under `env:` in
suite_contract.yaml (values may use `tests/...` repo-relative paths, resolved
to absolute here) so the spawned script sees the right roots.
"""
from __future__ import annotations

import os
import subprocess
import time
from pathlib import Path

from .model import CaseResult, SuiteResult
from .dotnet import repo_root


def _resolve_group_env(cwd: str, raw: dict) -> dict:
    """Resolve a group `env:` dict to absolute values and merge over os.environ."""
    env = dict(os.environ)
    sep = os.pathsep  # ";" on Windows, ":" on POSIX
    for k, v in raw.items():
        if k == "PYTHONPATH":
            # A `;`/`:`-separated list of repo-relative paths → resolve each, then
            # prepend to the inherited PYTHONPATH so the engine package root wins.
            resolved = [str(Path(cwd) / p) for p in str(v).split(":")]
            env[k] = sep.join(resolved) + sep + env.get("PYTHONPATH", "")
            continue
        value = str(v)
        # Repo-relative path resource (e.g. "tests/e2e/translation") → absolute.
        if value.startswith(("tests/", "./", "tests\\\\")):
            value = str(Path(cwd) / value)
        env[k] = value
    return env


def run(group: dict, timeout: int = 3600, quick: bool = False) -> SuiteResult:
    cwd = repo_root()
    script = group["script"]
    args = group.get("args", [])
    env = _resolve_group_env(cwd, group.get("env", {}))
    res = SuiteResult(layer="e2e", group=group.get("name", script))

    cmd = ["python", script] + list(args)
    try:
        t0 = time.time()
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, cwd=cwd, env=env)
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
