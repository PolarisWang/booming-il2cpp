from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
import tempfile
import time
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
SOLUTION_SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionMultiProject"


def load_perf_module():
    perf_module_path = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "perf.py"
    spec = importlib.util.spec_from_file_location("chaos_convert_perf_suite", perf_module_path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load perf module: {perf_module_path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module.evaluate_perf_suite


def main() -> int:
    evaluate_perf_suite = load_perf_module()
    with tempfile.TemporaryDirectory(prefix="convert-golden-multi-project-") as temp_dir:
        output_root = Path(temp_dir) / "output"
        started = time.perf_counter()
        completed = subprocess.run(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "--",
                "convert",
                str(SOLUTION_SUBJECT_ROOT),
                "--output",
                str(output_root),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )
        duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
        if completed.returncode != 0:
            sys.stderr.write(completed.stdout)
            sys.stderr.write(completed.stderr)
            return completed.returncode

        result = evaluate_perf_suite(
            repo_root=REPO_ROOT,
            suite="convert-golden-multi-project",
            host_platform="windows",
            metrics={"convertDurationMs": duration_ms},
            update_baseline=False,
        )
        sys.stdout.write(json.dumps(result, ensure_ascii=False, indent=2) + "\n")
        return 0 if str(result["regressionStatus"]) != "regressed" else 2


if __name__ == "__main__":
    raise SystemExit(main())
