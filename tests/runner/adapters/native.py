"""native adapter: builds via CMake and runs CTest.

For the contracts-native group this:
  1. configures with the preset (if build dir absent) — else reuses build dir
  2. builds the test targets
  3. runs `ctest --output-on-failure` and parses per-test pass/fail

'quick' relies on an already-configured build dir and just builds+runs.
"""
from __future__ import annotations

import re
import os
import subprocess
import time
from pathlib import Path

from .model import CaseResult, SuiteResult
from .dotnet import repo_root


def _run(cwd, args, timeout):
    try:
        # utf-8 with errors=replace — the Windows default (GBK) decode crashes on
        # UTF-8 output (e.g. ctest emitting non-ASCII method names / paths).
        p = subprocess.run(args, capture_output=True, text=True, timeout=timeout, cwd=cwd,
                           encoding="utf-8", errors="replace")
        return p.returncode, (p.stdout or "") + "\n" + (p.stderr or "")
    except subprocess.TimeoutExpired:
        return 124, "TIMEOUT after {}s".format(timeout)


def run(group: dict, timeout: int = 1800, quick: bool = False) -> SuiteResult:
    cwd = repo_root()
    build_dir = group.get("cmake_build_dir", "artifacts/native-test/debug")
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
    # Visual Studio is a multi-config generator; ctest needs -C to select the
    # configuration the executables were built with (default Debug).
    # -LE excludes long-running benchmark/stress/soak tests (hours each) which are
    # not appropriate for a standard gating run; a user can run them separately.
    # -j runs independent native test processes in parallel (huge wall-clock win for
    # ~200 tests); RESOURCE_LOCK (in the CMake factory) serializes the heavy BGC/GC
    # group so it never runs concurrently and starves its 30s phase-wait windows.
    # --timeout is a per-test cap: the heavy-but-legit GC/BGC/fuzz group can exceed
    # 600s once serialized (no contention), so budget generously.
    #
    # These four knobs are contract-driven (P1/P2) — they come from the group's
    # ctest_* fields in suite_contract.yaml so tuning an integration group never
    # requires touching this adapter; the defaults preserve the historical values.
    ctest_config = str(group.get("ctest_config", "Debug"))
    ctest_exclude = str(group.get("ctest_exclude", "benchmark|stress|soak"))
    ctest_parallel = int(group.get("cmake_parallel", 8))
    ctest_timeout = int(group.get("ctest_timeout", 3600))
    # Optional ctest -R regex to narrow a group to a subset of targets (e.g. a
    # GC-only correctness gate).  When set, it is ANDed with the label selection
    # below.  Same contract-driven philosophy as ctest_config/ctest_exclude.
    ctest_regex = str(group.get("ctest_regex", ""))
    # P1-2: stress/soak/benchmark as an INDEPENDENT run tier.  By default the
    # adapter excludes them (-LE) so the unit/CI gate stays fast.  Setting
    # CHAOS_GC_TEST_STRESS_ONLY=1 flips to -L "stress|soak|benchmark" so ONLY the
    # pressure tests run (drive the test_driver --stress-only flag).
    label_arg = None
    if os.environ.get("CHAOS_GC_TEST_STRESS_ONLY", "") in ("1", "true", "yes"):
        label_arg = ["-L", "stress|soak|benchmark"]
    else:
        label_arg = ["-LE", ctest_exclude]
    cmd = ["ctest", "--test-dir", build_dir, "-C", ctest_config,
           *label_arg, "-j", str(ctest_parallel),
           "--timeout", str(ctest_timeout),
           "--output-on-failure"]
    if ctest_regex:
        cmd += ["-R", ctest_regex]
    rc, out = _run(cwd, cmd, timeout)
    res.duration_s = time.time() - t0
    # Stash the raw ctest stdout so the driver can relay it (benchmark tiers emit
    # `BENCH,<name>,KEY=VAL` lines the nightly tee + collector must capture).
    res.raw_out = out

    if rc == 124:
        res.error = "TIMEOUT ctest"; return res

    # CTest output lines: "N/M Test #nn: name ....................  Passed  1.23 sec"
    # The N/M run-progress prefix (1/1, 2/5, ...) precedes "Test #nn:", so use
    # search (not match) — match anchors at line start and would drop every line.
    started = False
    for line in out.splitlines():
        m = re.search(r"Test\s+#\d+:\s+(\S+).*?(Passed|Failed|\*\*\*Failed)", line)
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
