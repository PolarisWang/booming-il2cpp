from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUN_ENTRYPOINT_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run.py"
PROJECT_WORKSPACE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "project_workspace.py"
DECLARED_PROOF_ID = (
    "declared-unit-test/"
    "HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateSkeletonProofEntry::Run()"
)


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def parse_last_json_line(output: str) -> dict[str, object]:
    lines = [line.strip() for line in output.splitlines() if line.strip()]
    if not lines:
        raise AssertionError("command produced no output")
    return json.loads(lines[-1])


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class HotUpdateHostPackDeclaredProofTests(unittest.TestCase):
    def test_hotupdate_host_pack_declared_proof_runs_through_workspace_hotupdate_host(self) -> None:
        workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_project_workspace_hotupdate_host_pack_declared_proof",
        )
        workspace_module.generate_subject_workspace(
            REPO_ROOT,
            "windows",
            {"id": "subject/HotUpdateHostPack"},
        )

        completed = run_checked(
            [
                sys.executable,
                str(RUN_ENTRYPOINT_PATH),
                "test",
                "declared-unit-test",
                "--id",
                DECLARED_PROOF_ID,
                "--json",
            ],
            cwd=REPO_ROOT,
        )
        final_event = parse_last_json_line(completed.stdout)
        self.assertEqual("final-summary", final_event["eventType"])
        self.assertEqual("ok", final_event["status"])
        payload = dict(final_event.get("payload") or {})
        self.assertEqual("ok", payload.get("finalStatus"))


if __name__ == "__main__":
    unittest.main()
