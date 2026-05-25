"""Soak orchestrator — automated long-running stress test framework.

Usage:
    python -m verification.soak.orchestrator                  # 24h default
    python -m verification.soak.orchestrator --minutes 5      # 5-minute smoke test
    python -m verification.soak.orchestrator --phase gc       # GC only
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
import time
from pathlib import Path

from soak.report import (
    DegradationCheck,
    SoakReport,
    SoakSnapshot,
    compute_degradation,
)

_PROJECT_ROOT = Path(__file__).resolve().parents[4]


def _find_config() -> Path:
    candidates = [
        _PROJECT_ROOT / "testing" / "foundation-dll" / "config" / "soak-config.yaml",
        Path("testing/foundation-dll/config/soak-config.yaml"),
    ]
    for c in candidates:
        if c.exists():
            return c
    return candidates[0]


def _load_config(path: Path) -> dict:
    import json
    text = path.read_text(encoding="utf-8")
    config: dict = {}
    current_phase: dict | None = None

    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if ":" in stripped and not stripped.startswith("-"):
            key, _, val = stripped.partition(":")
            key = key.strip()
            val = val.strip()
            if val:
                config[key] = _coerce(val)
        if stripped.startswith("- name:"):
            if current_phase:
                config.setdefault("phases", []).append(current_phase)
            current_phase = {"name": stripped.split(":", 1)[1].strip()}
        elif current_phase is not None and ":" in stripped:
            k, _, v = stripped.partition(":")
            current_phase[k.strip()] = _coerce(v.strip())

    if current_phase:
        config.setdefault("phases", []).append(current_phase)

    return config


def _coerce(val: str):
    if val.isdigit():
        return int(val)
    try:
        return float(val)
    except ValueError:
        pass
    if val.lower() in ("true", "yes"):
        return True
    if val.lower() in ("false", "no"):
        return False
    return val


def _get_process_metrics() -> dict[str, float]:
    try:
        import psutil
        proc = psutil.Process()
        mem = proc.memory_info()
        return {
            "rss_mb": mem.rss / (1024 * 1024),
            "vms_mb": mem.vms / (1024 * 1024),
            "cpu_percent": proc.cpu_percent(interval=0.1),
        }
    except ImportError:
        pass

    try:
        with open("/proc/self/status") as f:
            data = f.read()
        rss_kb = 0
        vm_kb = 0
        for line in data.splitlines():
            if line.startswith("VmRSS:"):
                rss_kb = int(line.split()[1])
            elif line.startswith("VmSize:"):
                vm_kb = int(line.split()[1])
        return {
            "rss_mb": rss_kb / 1024,
            "vms_mb": vm_kb / 1024,
            "cpu_percent": 0.0,
        }
    except (FileNotFoundError, PermissionError):
        pass

    return {"rss_mb": 0.0, "vms_mb": 0.0, "cpu_percent": 0.0}


def _discover_ctest_tests(label: str) -> list[str]:
    try:
        result = subprocess.run(
            ["ctest", "-N", "-L", label],
            capture_output=True, text=True, timeout=30,
            cwd=str(_PROJECT_ROOT),
        )
        tests = []
        for line in result.stdout.splitlines():
            if "Test #" in line and ":" in line:
                parts = line.split(":", 1)
                if len(parts) == 2:
                    tests.append(parts[1].strip())
        return tests
    except (subprocess.TimeoutExpired, FileNotFoundError):
        return []


def _run_ctest_test(test_name: str, timeout: int) -> tuple[int, str]:
    try:
        result = subprocess.run(
            ["ctest", "-R", f"^{test_name}$", "--output-on-failure", "-V"],
            capture_output=True, text=True, timeout=timeout,
            cwd=str(_PROJECT_ROOT),
        )
        output = result.stdout + result.stderr
        return result.returncode, output
    except subprocess.TimeoutExpired:
        return -1, f"TIMEOUT after {timeout}s"
    except FileNotFoundError:
        return -2, "ctest not found"


def _parse_test_output(output: str) -> tuple[int, int]:
    passed = 0
    failed = 0
    for line in output.splitlines():
        if "[  PASSED  ]" in line:
            parts = line.strip().split()
            for p in parts:
                if p.isdigit():
                    passed = int(p)
        if "[  FAILED  ]" in line:
            parts = line.strip().split()
            for p in parts:
                if p.isdigit():
                    failed = int(p)
    return passed, failed


class SoakOrchestrator:
    """Soak test orchestrator — runs stress test phases with metric sampling."""

    def __init__(self, config: dict, report_dir: Path, duration_min: int | None = None):
        self.config = config
        self.report_dir = report_dir
        self.total_duration_min = duration_min or config.get("total_duration_min", 1440)
        self.sampling_interval = config.get("sampling_interval_sec", 30)
        self.test_timeout = config.get("test_timeout_sec", 600)
        self.phases = config.get("phases", [])
        self._snapshots: list[SoakSnapshot] = []
        self._cum_passed = 0
        self._cum_failed = 0

    def run(self) -> int:
        total_sec = self.total_duration_min * 60
        report_path = self.report_dir / f"soak-{int(time.time())}.jsonl"
        degraded = False

        print(f"Soak start: {self.total_duration_min} min, report -> {report_path}")

        with SoakReport(report_path) as report:
            start_time = time.time()
            phase_start = start_time

            for phase in self.phases:
                phase_duration = phase["duration_min"] * 60
                label = phase.get("test_label", "stress")
                phase_name = phase["name"]
                phase_end = phase_start + phase_duration
                print(f"\n--- Phase {phase_name}: {phase['description']}")
                print(f"    label={label}, duration={phase['duration_min']}min")

                tests = _discover_ctest_tests(label)
                if not tests:
                    print(f"    [WARN] No tests found for label '{label}', skipping phase")
                    continue

                print(f"    discovered {len(tests)} tests")

                round_num = 0
                last_snapshot = time.time()

                while time.time() < phase_end:
                    round_num += 1
                    print(f"    Round {round_num} ({len(tests)} tests)")

                    for test_name in tests:
                        rc, output = _run_ctest_test(test_name, self.test_timeout)
                        p, f = _parse_test_output(output)
                        self._cum_passed += p
                        self._cum_failed += f

                        status = "PASS" if rc == 0 else "FAIL"
                        print(f"      [{status}] {test_name} (passed={p} failed={f}, rc={rc})")

                    now = time.time()
                    if now - last_snapshot >= self.sampling_interval:
                        metrics = _get_process_metrics()
                        snap = SoakSnapshot(
                            timestamp=time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime()),
                            elapsed_sec=now - start_time,
                            phase=phase_name,
                            rss_mb=metrics["rss_mb"],
                            vms_mb=metrics["vms_mb"],
                            cpu_percent=metrics["cpu_percent"],
                            tests_passed=self._cum_passed,
                            tests_failed=self._cum_failed,
                        )
                        self._snapshots.append(snap)
                        report.write_snapshot(snap)
                        last_snapshot = now

                for metric in ("rss_mb", "vms_mb", "tests_failed"):
                    check = compute_degradation(self._snapshots, metric)
                    report.write_degradation(check)
                    if not check.passed:
                        print(f"    [DEGRADATION] {check.warning}")
                        degraded = True
                    else:
                        print(f"    [{metric}] slope={check.slope:.3f} — ok")

                phase_start = phase_end

            elapsed = time.time() - start_time
            print(f"\n{'='*50}")
            print(f"Soak complete: {elapsed:.0f}s elapsed")
            print(f"Cumulative: {self._cum_passed} passed, {self._cum_failed} failed")
            if degraded:
                print("Degradation detected — review report for details")
            else:
                print("No degradation detected")
            print(f"Report: {report_path}")

        return 1 if degraded else 0


def main() -> int:
    parser = argparse.ArgumentParser(description="24h soak test orchestrator")
    parser.add_argument("--minutes", type=int, default=None, help="Override duration in minutes")
    parser.add_argument("--phase", type=str, default=None, help="Run only a specific phase by name")
    parser.add_argument("--report-dir", type=str, default=None, help="Report output directory")
    args = parser.parse_args()

    config_path = _find_config()
    if not config_path.exists():
        print(f"ERROR: config not found at {config_path}")
        return 1

    config = _load_config(config_path)
    if args.phase:
        config["phases"] = [p for p in config.get("phases", []) if p.get("name") == args.phase]
        if not config["phases"]:
            print(f"ERROR: phase '{args.phase}' not found in config")
            return 1

    report_base = Path(args.report_dir) if args.report_dir else _PROJECT_ROOT / config.get("output_dir", "testing/results/soak")

    orchestrator = SoakOrchestrator(config, report_base, duration_min=args.minutes)
    return orchestrator.run()


if __name__ == "__main__":
    sys.exit(main())
