"""native adapter: builds via CMake and runs CTest.

For the contracts-native group this:
  1. configures with the preset (if build dir absent) — else reuses build dir
  2. builds the test targets
  3. runs `ctest --output-on-failure` and parses per-test pass/fail

'quick' relies on an already-configured build dir and just builds+runs.
"""
from __future__ import annotations

import re
import subprocess
import time
from pathlib import Path

from .model import CaseResult, SuiteResult
from .dotnet import repo_root


def _run(cwd, args, timeout):
    try:
        p = subprocess.run(args, capture_output=True, text=True, timeout=timeout, cwd=cwd)
        return p.returncode, (p.stdout or "") + "\n" + (p.stderr or "")
    except subprocess.TimeoutExpired:
        return 124, "TIMEOUT after {}s".format(timeout)


def run(group: dict, timeout: int = 1800, quick: bool = False) -> SuiteResult:
    cwd = repo_root()
    build_dir = group.get("cmake_build_dir", "build/native")
    preset = group.get("cmake_preset", "windows-x64-reference")
    res = SuiteResult(layer="integration", group=group.get("name", "contracts-native"))

    # 1) configure if build dir missing or fast-mode with reusable dir
    if not (cwd and Path(cwd, build_dir).exists()) and not quick:
        cfg = ["cmake", "--preset", preset] if preset else ["cmake", "-S", ".", "-B", build_dir]
        rc, out = _run(cwd, cfg, timeout)
        if rc == 124:
            res.error = "TIMEOUT configure"; return res
        if rc != 0:
            res.error = "cmake configure failed: " + out[-1500:]
            return res

    # 2) build
    t0 = time.time()
    rc, out = _run(cwd, ["cmake", "--build", build_dir], timeout)
    if rc != 0 and rc != 124:
        res.error = "cmake build failed: " + out[-1500:]
        res.duration_s = time.time() - t0
        return res

    # 3) ctest
    rc, out = _run(cwd, ["ctest", "--test-dir", build_dir, "--output-on-failure"], timeout)
    res.duration_s = time.time() - t0

    if rc == 124:
        res.error = "TIMEOUT ctest"; return res

    # CTest output lines: "Test #N: name ............................   Passed  1.23 sec"
    started = False
    for line in out.splitlines():
        m = re.match(r"Test\s+#\d+:\s+(\S+).*?(Passed|Failed|\*\*\*Failed)", line)
        if not m:
            continue
        name, status = m.group(1), m.group(2)
        res.cases.append(CaseResult(name=name, passed=status == "Passed"))
    res.total = len(res.cases)
    res.passed = sum(1 for c in res.cases if c.passed)
    res.failed = len(res.cases) - res.passed
    if res.total == 0:
        res.error = "No CTest tests parsed (build produced none?):\n" + out[-1000:]
    return res
