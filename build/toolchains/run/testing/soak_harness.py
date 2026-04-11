from __future__ import annotations

import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

try:
    import psutil  # type: ignore
except ImportError:  # pragma: no cover - optional dependency
    psutil = None

try:
    from ..core.common import write_json
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json


def _utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def _sample_with_psutil(pid: int) -> dict[str, Any] | None:
    if psutil is None:
        return None
    try:
        process = psutil.Process(pid)
        cpu_times = process.cpu_times()
        return {
            "timestamp": _utc_timestamp(),
            "workingSetBytes": int(process.memory_info().rss),
            "cpuSeconds": round(float(cpu_times.user + cpu_times.system), 3),
        }
    except (psutil.NoSuchProcess, psutil.AccessDenied, OSError):
        return None


def _sample_windows_process(pid: int) -> dict[str, Any] | None:
    completed = subprocess.run(
        [
            "powershell",
            "-NoProfile",
            "-Command",
            (
                f"$process = Get-Process -Id {pid} -ErrorAction SilentlyContinue; "
                "if ($null -eq $process) { exit 1 }; "
                "Write-Output ($process.WorkingSet64.ToString() + ',' + $process.CPU.ToString())"
            ),
        ],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        return None

    parts = (completed.stdout or "").strip().split(",", maxsplit=1)
    if len(parts) != 2:
        return None

    try:
        return {
            "timestamp": _utc_timestamp(),
            "workingSetBytes": int(parts[0]),
            "cpuSeconds": round(float(parts[1]), 3),
        }
    except ValueError:
        return None


def _sample_posix_process(pid: int) -> dict[str, Any] | None:
    completed = subprocess.run(
        ["ps", "-p", str(pid), "-o", "rss=", "-o", "time="],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        return None

    raw_line = next((line.strip() for line in (completed.stdout or "").splitlines() if line.strip()), "")
    if not raw_line:
        return None

    parts = raw_line.split(maxsplit=1)
    if len(parts) != 2:
        return None

    try:
        working_set_bytes = int(parts[0]) * 1024
    except ValueError:
        return None

    time_parts = [int(part) for part in parts[1].split(":") if part.isdigit()]
    if len(time_parts) == 2:
        cpu_seconds = float(time_parts[0] * 60 + time_parts[1])
    elif len(time_parts) == 3:
        cpu_seconds = float(time_parts[0] * 3600 + time_parts[1] * 60 + time_parts[2])
    else:
        cpu_seconds = 0.0

    return {
        "timestamp": _utc_timestamp(),
        "workingSetBytes": working_set_bytes,
        "cpuSeconds": round(cpu_seconds, 3),
    }


def _sample_process(pid: int) -> dict[str, Any] | None:
    sample = _sample_with_psutil(pid)
    if sample is not None:
        return sample
    if sys.platform == "win32":
        return _sample_windows_process(pid)
    return _sample_posix_process(pid)


def _iteration_report(
    *,
    iteration_index: int,
    command: list[str],
    exit_code: int,
    wall_duration_seconds: float,
    samples: list[dict[str, Any]],
    stdout: str,
    stderr: str,
) -> dict[str, Any]:
    return {
        "iteration": iteration_index,
        "command": list(command),
        "status": "ok" if exit_code == 0 else "fail",
        "exitCode": exit_code,
        "wallDurationSeconds": round(wall_duration_seconds, 3),
        "sampleCount": len(samples),
        "peakWorkingSetBytes": max((int(sample["workingSetBytes"]) for sample in samples), default=None),
        "peakCpuSeconds": max((float(sample["cpuSeconds"]) for sample in samples), default=None),
        "stdout": stdout,
        "stderr": stderr,
        "samples": samples,
    }


def run_soak_harness(
    arguments: list[str],
    *,
    cwd: Path,
    duration_seconds: float,
    poll_interval_seconds: float = 1.0,
    max_iterations: int | None = None,
) -> dict[str, Any]:
    if not arguments:
        raise ValueError("soak harness requires a command to execute")
    if duration_seconds <= 0:
        raise ValueError("duration_seconds must be > 0")
    if poll_interval_seconds <= 0:
        raise ValueError("poll_interval_seconds must be > 0")

    run_started = time.monotonic()
    deadline = run_started + duration_seconds
    iterations: list[dict[str, Any]] = []
    total_samples = 0

    while time.monotonic() < deadline:
        if max_iterations is not None and len(iterations) >= max_iterations:
            break

        iteration_started = time.monotonic()
        process = subprocess.Popen(
            arguments,
            cwd=str(cwd),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="replace",
        )

        samples: list[dict[str, Any]] = []
        while True:
            sample = _sample_process(process.pid)
            if sample is not None:
                samples.append(sample)

            if process.poll() is not None:
                break

            time.sleep(poll_interval_seconds)

        stdout_text, stderr_text = process.communicate()
        iteration_report = _iteration_report(
            iteration_index=len(iterations) + 1,
            command=arguments,
            exit_code=int(process.returncode or 0),
            wall_duration_seconds=time.monotonic() - iteration_started,
            samples=samples,
            stdout=stdout_text or "",
            stderr=stderr_text or "",
        )
        iterations.append(iteration_report)
        total_samples += len(samples)

        if process.returncode != 0:
            break

    fail_count = sum(1 for iteration in iterations if str(iteration["status"]) != "ok")
    ok_count = len(iterations) - fail_count
    return {
        "reportVersion": "v1",
        "generatedAt": _utc_timestamp(),
        "status": "ok" if iterations and fail_count == 0 else "fail",
        "requestedDurationSeconds": duration_seconds,
        "actualDurationSeconds": round(time.monotonic() - run_started, 3),
        "pollIntervalSeconds": poll_interval_seconds,
        "maxIterations": max_iterations,
        "sampleCount": total_samples,
        "crashedIterations": fail_count,
        "peakWorkingSetBytes": max(
            (
                int(iteration["peakWorkingSetBytes"])
                for iteration in iterations
                if iteration.get("peakWorkingSetBytes") is not None
            ),
            default=None,
        ),
        "peakCpuSeconds": max(
            (
                float(iteration["peakCpuSeconds"])
                for iteration in iterations
                if iteration.get("peakCpuSeconds") is not None
            ),
            default=None,
        ),
        "statusCounts": {
            "totalIterations": len(iterations),
            "ok": ok_count,
            "fail": fail_count,
        },
        "iterations": iterations,
    }


def write_soak_harness_report(path: Path, payload: dict[str, Any]) -> None:
    write_json(path, payload)
