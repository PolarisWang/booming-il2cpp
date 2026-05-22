from __future__ import annotations

import contextlib
import importlib.util
import io
import sys
import types
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCHMARK_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "benchmark.py"


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


class BenchmarkCommandTests(unittest.TestCase):
    def test_dashboard_defaults_to_verification_projection_root(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_run_benchmark_dashboard_default")
        dash_mod = types.SimpleNamespace(generate=lambda repo_root, output_path: None, update_docs=lambda repo_root: None)
        output = io.StringIO()

        def fake_load(name: str, path: Path):
            del path
            modules = {
                "benchmark_records": types.SimpleNamespace(),
                "device_detector": types.SimpleNamespace(),
                "benchmark_dashboard_generator": dash_mod,
                "subjects": types.SimpleNamespace(),
            }
            return modules[name]

        with patch.object(benchmark_module, "_load", side_effect=fake_load):
            with contextlib.redirect_stdout(output):
                exit_code = benchmark_module.dispatch(["--dashboard"], REPO_ROOT, "windows")

        self.assertEqual(0, exit_code)
        self.assertIn(
            "verification/projections/benchmark/dashboard.html",
            output.getvalue().replace("\\", "/"),
        )

    def test_record_mode_does_not_refresh_benchmark_projection_directly(self) -> None:
        benchmark_module = load_module(BENCHMARK_MODULE_PATH, "chaos_run_benchmark_record_no_projection_refresh")
        dash_mod = types.SimpleNamespace(update_docs=lambda repo_root, subject_id=None: None)
        detector_mod = types.SimpleNamespace(load_or_detect=lambda repo_root: {"id": "fixture-device", "name": "Fixture Device"})
        subjects_mod = types.SimpleNamespace(
            load_subject_manifest=lambda repo_root, subject_id: {"subjectId": subject_id},
        )
        output = io.StringIO()

        def fake_load(name: str, path: Path):
            del path
            modules = {
                "benchmark_records": types.SimpleNamespace(),
                "device_detector": detector_mod,
                "benchmark_dashboard_generator": dash_mod,
                "subjects": subjects_mod,
            }
            return modules[name]

        with patch.object(benchmark_module, "_load", side_effect=fake_load):
            with patch.object(benchmark_module, "_supported_benchmark_modes", return_value=["managed"]):
                with patch.object(benchmark_module, "_discover_declared_benchmark_cases", return_value=[]):
                    with patch.object(
                        benchmark_module,
                        "_run_pipeline_and_record",
                        return_value={
                            "record": {
                                "metrics": {"meanDurationMs": 1.0},
                            }
                        },
                    ):
                        with patch.object(benchmark_module, "_print_result"):
                            with patch.object(dash_mod, "update_docs") as update_docs_mock:
                                with contextlib.redirect_stdout(output):
                                    exit_code = benchmark_module.dispatch(
                                        ["--subject", "SolutionCorePack", "--record"],
                                        REPO_ROOT,
                                        "windows",
                                    )

        self.assertEqual(0, exit_code)
        update_docs_mock.assert_not_called()
        self.assertIn("run test inventory --json", output.getvalue())


if __name__ == "__main__":
    unittest.main()
