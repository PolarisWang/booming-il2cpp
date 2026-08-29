"""Unit tests for the unified test driver (tests/runner/test_driver.py) and its
native adapter contract-driven configuration.

Covers the P1–P5 optimization work on the test entry:
  P1  native ctest per-test timeout comes from the contract, not a hardcode 1800
  P2  native ctest -j / -C / -LE / --timeout come from group.ctest_* fields,
      falling back to historical defaults when absent
  P3  the no-dependency _fallback_yaml parser matches yaml.safe_load field-for-
      field (incl. nested groups, ints, inline lists, inline comments)
  P4  explicit regression gate: a NEW failure (not in baseline) flips the group
      red even when all old failures are known; stale baseline entries that now
      pass are flagged for pruning
  P5  --cases includes per-case detail in the JSON report; off by default
"""
from __future__ import annotations

import contextlib
import importlib.util
import io
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

REPO_ROOT = Path(__file__).resolve().parents[3]
RUNNER_DIR = REPO_ROOT / "tests" / "runner"

sys.path.insert(0, str(RUNNER_DIR))  # so the driver's `from adapters import ...` resolves


def _load(path: str, name: str):
    spec = importlib.util.spec_from_file_location(name, str(RUNNER_DIR / path))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


DRIVER = _load("test_driver.py", "test_driver")
# adapters is a real package (has __init__.py); import it normally so
# native.py's `from .model import ...` relative import resolves.
import adapters.native as _native_pkg  # noqa: E402
NATIVE = _native_pkg
REAL_CONTRACT_TEXT = (REPO_ROOT / "tests" / "suite_contract.yaml").read_text(encoding="utf-8")


class FallbackYamlCompletenessTests(unittest.TestCase):
    """P3 — fallback parser must not silently drop any contract field."""

    def test_fallback_matches_safe_load_on_real_contract(self):
        try:
            import yaml  # type: ignore
            ref = yaml.safe_load(REAL_CONTRACT_TEXT)
        except Exception:  # pragma: no cover - env without PyYAML
            self.skipTest("PyYAML not available")
        fallback = DRIVER._fallback_yaml(REAL_CONTRACT_TEXT)
        self.assertEqual(fallback, ref)
        self.assertEqual(DRIVER._contract_shape(fallback), DRIVER._contract_shape(ref))

    def test_fallback_captures_nested_group_fields(self):
        frag = (
            "layers:\n"
            "  e2e:\n"
            "    adapter: python\n"
            "    groups:\n"
            "      - name: foundation-dll-chunk\n"
            "        script: testing/x.py\n"
            "        args: [--smoke]\n"
            "        slow: true\n"
            "        timeout: 5400\n"
        )
        g = DRIVER._fallback_yaml(frag)["layers"]["e2e"]["groups"][0]
        self.assertEqual(g["name"], "foundation-dll-chunk")
        self.assertEqual(g["args"], ["--smoke"])
        self.assertIs(g["slow"], True)
        self.assertEqual(g["timeout"], 5400)
        self.assertIsInstance(g["timeout"], int)

    def test_fallback_strips_inline_comments_keep_ints(self):
        frag = ("layers:\n  integration:\n    adapter: native\n    groups:\n"
                "      - name: g\n        ctest_timeout: 3600  # per-test cap\n")
        g = DRIVER._fallback_yaml(frag)["layers"]["integration"]["groups"][0]
        self.assertEqual(g["ctest_timeout"], 3600)
        self.assertIsInstance(g["ctest_timeout"], int)

    def test_load_contract_does_not_raise(self):
        c = DRIVER.load_contract()
        self.assertIn("unit", c["layers"])
        self.assertEqual(c["layers"]["integration"]["groups"][0]["name"], "contracts-native")


class NativeAdapterContractDrivenTests(unittest.TestCase):
    """P1 + P2 — native ctest knobs come from the contract, defaults preserved."""

    def _run_and_capture(self, group: dict):
        captured = []

        def fake_run(cwd, args, timeout, **kw):
            captured.append(list(args))
            if args[0] == "cmake":
                return 0, "ok"
            if args[0] == "ctest":
                return 0, ("Test #1: alpha .........   Passed  1.23 sec\n"
                           "Test #2: beta .........   ***Failed 0.50 sec")
            return 0, ""

        group = {**group, "cmake_build_dir": "artifacts/native-test/debug",
                 "cmake_preset": "windows-x64-reference"}
        with mock.patch.object(NATIVE, "_run", side_effect=fake_run):
            res = NATIVE.run(group, timeout=3600, quick=True)
        ctest_args = next(a for a in captured if a and a[0] == "ctest")
        return ctest_args, res

    def _flag_val(self, ctest_args, flag):
        return ctest_args[ctest_args.index(flag) + 1]

    def test_ctest_timeout_from_contract_p1(self):
        # P1: ctest --timeout comes from group.ctest_timeout (999), not the 1800 hardcode.
        ctest_args, res = self._run_and_capture({"name": "g", "ctest_timeout": 999})
        self.assertEqual(self._flag_val(ctest_args, "--timeout"), "999")
        self.assertNotIn("1800", ctest_args)
        self.assertEqual(res.total, 2)
        self.assertEqual(res.failed, 1)

    def test_ctest_defaults_when_fields_absent(self):
        # P2: absent fields fall back to historical defaults.
        ctest_args, _ = self._run_and_capture({"name": "g"})
        self.assertEqual(self._flag_val(ctest_args, "-j"), "8")
        self.assertEqual(self._flag_val(ctest_args, "-C"), "Debug")
        self.assertEqual(self._flag_val(ctest_args, "-LE"), "benchmark|stress|soak")

    def test_ctest_overrides_from_contract_p2(self):
        ctest_args, _ = self._run_and_capture({
            "name": "g", "cmake_parallel": 3, "ctest_config": "Release",
            "ctest_exclude": "slow|huge", "ctest_timeout": 7200})
        self.assertEqual(self._flag_val(ctest_args, "-j"), "3")
        self.assertEqual(self._flag_val(ctest_args, "-C"), "Release")
        self.assertEqual(self._flag_val(ctest_args, "-LE"), "slow|huge")
        self.assertEqual(self._flag_val(ctest_args, "--timeout"), "7200")


class RegressionGateTests(unittest.TestCase):
    """P4 — explicit regression gate + stale-known diagnostics via real main()."""

    def _build_suite(self, cases, total, passed, failed):
        from adapters.model import CaseResult, SuiteResult
        return SuiteResult(layer="unit", group="g", total=total, passed=passed,
                           failed=failed, cases=[CaseResult(**c) for c in cases])

    def _run_main(self, suite, known, cases_flag=False):
        with tempfile.TemporaryDirectory() as td:
            json_path = str(Path(td) / "out.json")
            base_known = DRIVER.load_known_failures
            base_contract = DRIVER.load_contract
            contract = {"layers": {"unit": {"adapter": "dotnet",
                                            "groups": [{"name": "g"}]}}}
            DRIVER.load_known_failures = lambda layer: set(known)
            DRIVER.load_contract = lambda: contract
            try:
                with mock.patch.object(DRIVER, "run_group", return_value=suite):
                    with mock.patch.object(DRIVER, "ROOT", REPO_ROOT):
                        argv = ["test_driver.py", "--json", json_path]
                        if cases_flag:
                            argv.append("--cases")
                        with mock.patch.object(sys, "argv", argv):
                            with contextlib.redirect_stdout(io.StringIO()):
                                rc = DRIVER.main()
                with open(json_path, encoding="utf-8") as f:
                    report = json.load(f)
            finally:
                DRIVER.load_known_failures = base_known
                DRIVER.load_contract = base_contract
            return rc, report

    def test_new_failure_flips_gate_red(self):
        suite = self._build_suite(
            [{"name": "knownA", "passed": False}, {"name": "newFail", "passed": False}],
            total=2, passed=0, failed=2)
        rc, report = self._run_main(suite, known={"knownA"})
        g = report["layers"]["unit"]["groups"]["g"]
        self.assertFalse(g["ok"], "new failure must flip gate red")
        self.assertEqual(g["unexpected"], ["newFail"])

    def test_all_known_passes_gate(self):
        suite = self._build_suite(
            [{"name": "knownA", "passed": False}, {"name": "knownB", "passed": False}],
            total=2, passed=0, failed=2)
        rc, report = self._run_main(suite, known={"knownA", "knownB"})
        g = report["layers"]["unit"]["groups"]["g"]
        self.assertTrue(g["ok"])
        self.assertEqual(g["unexpected"], [])

    def test_stale_known_diagnostic(self):
        # baseline says staleC is known-fail, but it passes this run => pruned.
        suite = self._build_suite(
            [{"name": "staleC", "passed": True, "duration_s": 0.1},
             {"name": "knownB", "passed": False}],
            total=2, passed=1, failed=1)
        rc, report = self._run_main(suite, known={"staleC", "knownB"})
        g = report["layers"]["unit"]["groups"]["g"]
        self.assertTrue(g["ok"])  # only known failure remains
        self.assertEqual(g["stale_known"], ["staleC"])


class PerCaseReportTests(unittest.TestCase):
    """P5 — per-case detail appears only with --cases."""

    def _case_suite(self):
        from adapters.model import CaseResult, SuiteResult
        return SuiteResult(layer="unit", group="g", total=2, passed=1, failed=1,
                           cases=[CaseResult(name="ok", passed=True, duration_s=0.1),
                                  CaseResult(name="bad", passed=False, message="boom")])

    def _run(self, cases_flag):
        with tempfile.TemporaryDirectory() as td:
            json_path = str(Path(td) / "out.json")
            suite = self._case_suite()
            base_contract = DRIVER.load_contract
            contract = {"layers": {"unit": {"adapter": "dotnet",
                                            "groups": [{"name": "g"}]}}}
            DRIVER.load_contract = lambda: contract
            try:
                with mock.patch.object(DRIVER, "run_group", return_value=suite):
                    with mock.patch.object(DRIVER, "ROOT", REPO_ROOT):
                        argv = ["test_driver.py", "--json", json_path]
                        if cases_flag:
                            argv.append("--cases")
                        with mock.patch.object(sys, "argv", argv):
                            with contextlib.redirect_stdout(io.StringIO()):
                                DRIVER.main()
            finally:
                DRIVER.load_contract = base_contract
            with open(json_path, encoding="utf-8") as f:
                return json.load(f)

    def test_cases_included_with_flag(self):
        report = self._run(cases_flag=True)
        g = report["layers"]["unit"]["groups"]["g"]
        names = [c["name"] for c in g["cases"]]
        self.assertIn("ok", names)
        self.assertIn("bad", names)
        # failure detail preserved
        bad = next(c for c in g["cases"] if c["name"] == "bad")
        self.assertEqual(bad["msg"], "boom")

    def test_cases_absent_without_flag(self):
        report = self._run(cases_flag=False)
        g = report["layers"]["unit"]["groups"]["g"]
        self.assertNotIn("cases", g)


if __name__ == "__main__":
    unittest.main()
