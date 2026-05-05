"""Verify the cross-language trace system produces correct JSONL records.

Tests cover:
  - trace_init idempotency
  - Required fields in every record (t, l, o, traceId)
  - trace_call decorator produces enter/exit phases with duration
  - Span ID generation and parentSpanId linkage
  - Environment variable export (CHAOS_TRACE_PATH, CHAOS_TRACE_ID)
"""

from __future__ import annotations

import json
import os
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]


class TestTraceSystem(unittest.TestCase):

    def setUp(self):
        # Fresh import to get uninitialized module state
        import importlib
        import build.toolchains.run.testing.trace as trace_mod
        self.trace = trace_mod
        # Reset module state by re-importing
        importlib.reload(self.trace)

        # Use a unique stage per test
        self._test_count = getattr(self.__class__, "_test_counter", 0) + 1
        self.__class__._test_counter = self._test_count
        self.stage = f"trace-test-{self._test_count}"

    def _trace_path(self) -> Path:
        """Resolve the trace file path from the session."""
        sid = self.trace.get_session_id()
        if not sid:
            return None
        return REPO_ROOT / "artifacts" / "logs" / "trace" / sid / "trace.jsonl"

    def _read_records(self) -> list[dict]:
        tp = self._trace_path()
        if not tp or not tp.exists():
            return []
        with open(tp, encoding="utf-8") as f:
            return [json.loads(line) for line in f if line.strip()]

    # ── Tests ────────────────────────────────────────────────────────────

    def test_init_creates_trace_file(self):
        """trace_init() creates trace.jsonl and exports env vars."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)

        tp = self._trace_path()
        self.assertIsNotNone(tp, "session ID should be set")
        self.assertTrue(tp.exists(), f"trace file should exist at {tp}")

        # Verify env vars exported
        self.assertIn("CHAOS_TRACE_PATH", os.environ)
        self.assertIn("CHAOS_TRACE_ID", os.environ)
        self.assertTrue(os.environ["CHAOS_TRACE_PATH"].endswith("trace.jsonl"))
        self.assertEqual(len(os.environ["CHAOS_TRACE_ID"]), 16)

    def test_init_is_idempotent(self):
        """Second trace_init() should return the same session ID."""
        sid1 = self.trace.trace_init(REPO_ROOT, stage=self.stage)
        sid2 = self.trace.trace_init(REPO_ROOT, stage="other")
        self.assertEqual(sid1, sid2, "trace_init should be idempotent")

    def test_session_start_record(self):
        """Trace file should start with a session_start record."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)
        records = self._read_records()

        self.assertGreaterEqual(len(records), 1)
        first = records[0]
        self.assertEqual(first.get("o"), "session_start")
        self.assertEqual(first.get("traceId"), os.environ["CHAOS_TRACE_ID"])

    def test_required_fields(self):
        """Every trace record must have t, l, o, traceId."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)
        self.trace.trace("test_op", key="value")

        records = self._read_records()
        for rec in records:
            with self.subTest(record=rec.get("o", "?")):
                self.assertIn("t", rec, f"missing timestamp in {rec}")
                self.assertIn("l", rec, f"missing language tag in {rec}")
                self.assertIn("o", rec, f"missing operation in {rec}")
                self.assertIn("traceId", rec, f"missing traceId in {rec}")
                self.assertIn("f", rec, f"missing source location in {rec}")

    def test_trace_call_phases(self):
        """@trace_call should produce enter → exit phases with duration."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)

        @self.trace.trace_call("test_func", stage=self.stage)
        def dummy_func():
            return 42

        result = dummy_func()
        self.assertEqual(result, 42)

        records = self._read_records()
        # Find the enter and exit records for test_func
        enters = [r for r in records if r.get("o") == "test_func" and r.get("phase") == "enter"]
        exits = [r for r in records if r.get("o") == "test_func" and r.get("phase") == "exit"]

        self.assertEqual(len(enters), 1, "should have one enter")
        self.assertEqual(len(exits), 1, "should have one exit")
        self.assertIn("dur_ms", exits[0], "exit should have duration")
        self.assertIn("result", exits[0], "exit should have result")

    def test_trace_call_exception(self):
        """@trace_call on a failing function should produce exception phase."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)

        @self.trace.trace_call("failing_func", stage=self.stage)
        def will_fail():
            raise ValueError("test error")

        with self.assertRaises(ValueError):
            will_fail()

        records = self._read_records()
        exceptions = [r for r in records if r.get("o") == "failing_func" and r.get("phase") == "exception"]

        self.assertEqual(len(exceptions), 1, "should have one exception record")
        self.assertEqual(exceptions[0].get("exception"), "ValueError")
        self.assertIn("dur_ms", exceptions[0])

    def test_span_hierarchy(self):
        """Nested @trace_call should create parentSpanId linkage."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)

        @self.trace.trace_call("outer", stage=self.stage)
        def outer_func():
            inner_func()
            return "done"

        @self.trace.trace_call("inner", stage=self.stage)
        def inner_func():
            return 1

        outer_func()

        records = self._read_records()
        enters = [r for r in records if r.get("phase") == "enter"]

        outer_enter = next(r for r in enters if r.get("o") == "outer")
        inner_enter = next(r for r in enters if r.get("o") == "inner")

        # Outer should have no parentSpanId; inner should have outer's spanId as parent
        self.assertIn("spanId", outer_enter, "outer should have spanId")
        self.assertIn("spanId", inner_enter, "inner should have spanId")
        self.assertIn("parentSpanId", inner_enter, "inner should have parentSpanId")
        self.assertEqual(
            inner_enter["parentSpanId"],
            outer_enter["spanId"],
            "inner's parent should be outer's span",
        )

    def test_trace_with_run_id(self):
        """Records should include runId when set via set_run_id()."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)
        self.trace.set_run_id("test-run-001")
        self.trace.trace("op_with_run")

        records = self._read_records()
        target = [r for r in records if r.get("o") == "op_with_run"]
        self.assertEqual(len(target), 1)
        self.assertEqual(target[0].get("runId"), "test-run-001")

    def test_csharp_compatible_format(self):
        """Record fields should match schema expected by C# ChaosTrace."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)
        self.trace.trace("format_check")

        records = self._read_records()
        for rec in records:
            # Language tag should be one of the agreed values
            self.assertIn(rec.get("l"), {"python", "cs", "cpp"})
            # Timestamp should be ISO 8601 UTC
            t = rec.get("t", "")
            self.assertTrue(t.endswith("Z"), f"timestamp should end with Z: {t}")
            # Source location should exist
            f = rec.get("f", "")
            self.assertIn(":", f, f"source location should have line number: {f}")

    def test_no_trace_no_crash(self):
        """trace() should be a no-op when trace is not initialized."""
        # Don't call trace_init
        self.trace.trace("orphan_op")
        # Should not crash — trace_path is None so _trace_write is skipped
        self.assertIsNone(self.trace._TRACE_PATH)

    def test_batch_runner_trace_calls(self):
        """Verify batch runners' trace operations match expected naming."""
        self.trace.trace_init(REPO_ROOT, stage="batch-native-aot")
        self.trace.trace("batch_start", assembly="System.Private.CoreLib", family_count=5)
        self.trace.trace("family_passed", family="convert-char", method_count=18)
        self.trace.trace("compile_ok", family="convert-char", variant="genuine")

        records = self._read_records()
        ops = [r.get("o") for r in records]

        self.assertIn("batch_start", ops)
        self.assertIn("family_passed", ops)
        self.assertIn("compile_ok", ops)

    def test_driver_trace_calls(self):
        """Verify driver trace operation naming convention."""
        self.trace.trace_init(REPO_ROOT, stage="codegen")
        self.trace.trace("driver.main", command="convert")
        self.trace.trace("driver.convert.pipeline", stage="convert")
        self.trace.trace("driver.emit_native_aot", stage="codegen")

        records = self._read_records()
        ops = [r.get("o") for r in records]
        self.assertIn("driver.main", ops)
        self.assertIn("driver.convert.pipeline", ops)
        self.assertIn("driver.emit_native_aot", ops)

    def test_verification_kernel_trace_calls(self):
        """Verify verification kernel trace operation naming."""
        self.trace.trace_init(REPO_ROOT, stage="kernel")
        self.trace.trace("kernel.build_snapshot", assembly_name="System.Private.CoreLib", family_count=10)
        self.trace.trace("kernel.evaluate_native_proof", family_id="convert-char", gate_code="native-proof")

        records = self._read_records()
        ops = [r.get("o") for r in records]
        self.assertIn("kernel.build_snapshot", ops)
        self.assertIn("kernel.evaluate_native_proof", ops)

    def test_runtime_trace_calls(self):
        """Verify TestFramework.Runtime trace operation naming."""
        self.trace.trace_init(REPO_ROOT, stage="runtime")
        self.trace.trace("runtime.start", stage="runtime")
        self.trace.trace("proof_runner.run", method="TestMethod")
        self.trace.trace("proof_runner.invoke:enter", stage="proof")
        self.trace.trace("proof_runner.invoke:exit", stage="proof", dur_ms=12.5)

        records = self._read_records()
        ops = [r.get("o") for r in records]
        self.assertIn("runtime.start", ops)
        self.assertIn("proof_runner.run", ops)
        self.assertIn("proof_runner.invoke:enter", ops)
        self.assertIn("proof_runner.invoke:exit", ops)

    def test_session_start_pointer(self):
        """trace_init() should write artifacts/logs/trace/last.json."""
        self.trace.trace_init(REPO_ROOT, stage=self.stage)
        pointer_path = REPO_ROOT / "artifacts" / "logs" / "trace" / "last.json"
        self.assertTrue(pointer_path.exists())
        pointer = json.loads(pointer_path.read_text(encoding="utf-8"))
        self.assertIn("sessionId", pointer)
        self.assertIn("traceId", pointer)
        self.assertIn("tracePath", pointer)


if __name__ == "__main__":
    unittest.main()
