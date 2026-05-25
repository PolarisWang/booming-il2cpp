"""Tests for context.py — dataclass construction, serialization."""

import json
from pathlib import Path

from orchestration.context import FamilyContext, StageResult, UnifiedReport


class TestStageResult:
    def test_default_construction(self):
        sr = StageResult(stage="fact", status="passed")
        assert sr.stage == "fact"
        assert sr.status == "passed"
        assert sr.summary == ""
        assert sr.details == {}
        assert sr.errors == []
        assert sr.duration_ms == 0

    def test_to_dict_roundtrip(self):
        sr = StageResult(
            stage="audit", status="passed",
            summary="All checks passed",
            details={"violations": []},
            duration_ms=1500,
        )
        d = sr.to_dict()
        assert d["stage"] == "audit"
        assert d["status"] == "passed"
        assert d["summary"] == "All checks passed"
        assert d["details"]["violations"] == []
        assert d["duration_ms"] == 1500


class TestFamilyContext:
    def test_default_construction(self):
        ctx = FamilyContext(slug="convert-char", assembly="System.Private.CoreLib", family_dir=Path("/tmp"))
        assert ctx.slug == "convert-char"
        assert ctx.mode == "standard"
        assert ctx.skip_stages == set()
        assert ctx.codegen_mode is None

    def test_properties(self):
        ctx = FamilyContext(slug="test", assembly="Test.Asm", family_dir=Path("/base/test"))
        assert ctx.contract_path == Path("/base/test/contract.json")
        assert ctx.native_dir == Path("/base/test/native")
        assert ctx.managed_dir == Path("/base/test/managed")
        assert ctx.entry_exe_path == Path("/base/test/native/entry.exe")

    def test_results_dir(self):
        ctx = FamilyContext(
            slug="convert-char",
            assembly="System.Private.CoreLib",
            family_dir=Path("/testing/foundation-dll/System.Private.CoreLib/convert-char"),
        )
        expected = Path("/testing/results/foundation-dll/System.Private.CoreLib/convert-char")
        assert ctx.results_dir == expected

    def test_strict_mode(self):
        ctx = FamilyContext(
            slug="strict-test", assembly="Test.Asm",
            family_dir=Path("/tmp"), mode="strict",
            skip_stages={"benchmark"},
        )
        assert ctx.mode == "strict"
        assert "benchmark" in ctx.skip_stages

    def test_codegen_mode(self):
        ctx = FamilyContext(
            slug="jit-test", assembly="Test.Asm",
            family_dir=Path("/tmp"), codegen_mode="jit",
        )
        assert ctx.codegen_mode == "jit"


class TestUnifiedReport:
    def test_default_status(self):
        report = UnifiedReport(family="test", assembly="Test.Asm")
        assert report.overall_status == "pending"
        assert report.stages == {}
        assert report.coverage == {}

    def test_to_dict(self):
        report = UnifiedReport(
            family="convert-char", assembly="System.Private.CoreLib",
            overall_status="passed",
            coverage={"stagePassRate": 100.0},
        )
        d = report.to_dict()
        assert d["family"] == "convert-char"
        assert d["overall_status"] == "passed"
        assert d["coverage"]["stagePassRate"] == 100.0

    def test_to_json(self):
        report = UnifiedReport(family="test", assembly="Test.Asm", overall_status="passed")
        j = report.to_json()
        parsed = json.loads(j)
        assert parsed["family"] == "test"
        assert parsed["overall_status"] == "passed"
