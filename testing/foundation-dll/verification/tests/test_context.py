"""Tests for verification.orchestration.context — ChunkContext and StageResult."""

from pathlib import Path
import json
import sys
import tempfile

# Ensure foundation-dll is on sys.path
_HERE = Path(__file__).resolve().parent  # verification/tests/
_VERIFICATION = _HERE.parent  # verification/
from verification._path import foundation_root
_FOUNDATION_DLL = foundation_root()# testing/foundation-dll/
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.orchestration.context import StageResult, ChunkContext, UnifiedReport


class TestStageResult:
    """StageResult is the most-used data model — verify construction and serialization."""

    def test_default_construction(self):
        sr = StageResult(stage="build", status="passed")
        assert sr.stage == "build"
        assert sr.status == "passed"
        assert sr.summary == ""
        assert sr.details == {}
        assert sr.errors == []
        assert sr.duration_ms == 0
        assert not sr.value_suspicious

    def test_full_construction(self):
        sr = StageResult(
            stage="fact",
            status="failed",
            summary="3/100 methods failed",
            details={"total": 100, "passed": 97, "failed": 3},
            errors=["Method 42: STATUS_ACCESS_VIOLATION"],
            duration_ms=1234,
            value_suspicious=True,
        )
        assert sr.stage == "fact"
        assert sr.status == "failed"
        assert sr.details["passed"] == 97
        assert len(sr.errors) == 1

    def test_to_dict_roundtrip(self):
        sr = StageResult(
            stage="benchmark",
            status="passed",
            summary="All good",
            duration_ms=5000,
        )
        d = sr.to_dict()
        assert d["stage"] == "benchmark"
        assert d["status"] == "passed"
        assert d["duration_ms"] == 5000
        # Ensure it's JSON-serializable
        json.dumps(d)

    def test_all_status_values(self):
        for status in ("passed", "failed", "skipped", "error"):
            sr = StageResult(stage="build", status=status)
            assert sr.status == status

    def test_empty_errors_list(self):
        sr = StageResult(stage="build", status="passed")
        assert sr.errors == []


class TestChunkContext:
    """ChunkContext drives all pipeline operations — verify path derivation."""

    def test_default_construction(self):
        ctx = ChunkContext(
            slug="numerics",
            assembly="System.Private.CoreLib",
            chunk_dir=Path("/tmp/chunks/numerics"),
            foundation_dir=Path("/tmp"),
        )
        assert ctx.slug == "numerics"
        assert ctx.assembly == "System.Private.CoreLib"
        assert ctx.mode == "standard"
        assert not ctx.verbose
        assert ctx.stage_timeout_seconds == 0

    def test_managed_dir_derivation(self):
        ctx = ChunkContext(
            slug="system",
            assembly="Test",
            chunk_dir=Path("/base/chunks/system"),
            foundation_dir=Path("/base"),
        )
        assert ctx.managed_dir == Path("/base/chunks/system/managed/subjects")
        assert ctx.native_dir == Path("/base/chunks/system/native")
        assert ctx.results_dir == Path("/base/chunks/system/results")
        assert ctx.entry_exe_path == Path("/base/chunks/system/native/entry.exe")

    def test_assembly_dirs_default_empty(self):
        ctx = ChunkContext(
            slug="collections",
            assembly="System.Collections",
            chunk_dir=Path("/c"),
            foundation_dir=Path("/f"),
        )
        assert ctx.assembly_dirs == []

    def test_assembly_dirs_custom(self):
        ctx = ChunkContext(
            slug="crypto",
            assembly="System.Security.Cryptography",
            chunk_dir=Path("/c"),
            foundation_dir=Path("/f"),
            assembly_dirs=["/f/crypto-refs"],
        )
        assert len(ctx.assembly_dirs) == 1

    def test_selected_properties_on_real_chunk(self):
        """Verify property paths are consistent with each other."""
        base = Path("/tmp/foundation")
        ctx = ChunkContext(
            slug="system-3",
            assembly="System.Private.CoreLib",
            chunk_dir=base / "chunks" / "system-3",
            foundation_dir=base,
        )
        assert ctx.dll_manifest_dir == base / "_dll"
        assert ctx.namespace_partition_path == base / "_dll" / "namespace-partition.json"
        assert ctx.subjects_dll_path == ctx.managed_dir / "CombinedSubjects.dll"
        assert ctx.subjects_metadata_path == ctx.managed_dir / "subjects.metadata.json"


class TestUnifiedReport:
    """UnifiedReport is the pipeline output artifact — verify serialization."""

    def test_default_construction(self):
        r = UnifiedReport(chunk="numerics", assembly="System.Private.CoreLib")
        assert r.overall_status == "pending"
        assert r.stages == {}

    def test_to_dict_and_to_json(self):
        r = UnifiedReport(
            chunk="system",
            assembly="Test",
            overall_status="passed",
            duration_ms=15000,
        )
        d = r.to_dict()
        assert d["overall_status"] == "passed"
        # to_json should produce valid JSON
        parsed = json.loads(r.to_json())
        assert parsed["chunk"] == "system"

    def test_load_from_file_valid(self):
        with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False, encoding="utf-8") as f:
            json.dump({"chunk": "io", "assembly": "Test", "overall_status": "failed"}, f)
            p = Path(f.name)
        try:
            r = UnifiedReport.load_from_file(p)
            assert r is not None
            assert r.chunk == "io"
            assert r.overall_status == "failed"
        finally:
            p.unlink(missing_ok=True)

    def test_load_from_file_missing(self):
        r = UnifiedReport.load_from_file(Path("/nonexistent/report.json"))
        assert r is None

    def test_load_from_file_invalid_json(self):
        with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False, encoding="utf-8") as f:
            f.write("{broken json")
            p = Path(f.name)
        try:
            r = UnifiedReport.load_from_file(p)
            assert r is None
        finally:
            p.unlink(missing_ok=True)

    def test_load_from_file_with_stages(self):
        data = {
            "chunk": "reflection",
            "assembly": "Test",
            "overall_status": "passed",
            "stages": {
                "build": {"stage": "build", "status": "passed"},
                "fact": {"stage": "fact", "status": "passed"},
            },
        }
        with tempfile.NamedTemporaryFile(mode="w", suffix=".json", delete=False, encoding="utf-8") as f:
            json.dump(data, f)
            p = Path(f.name)
        try:
            r = UnifiedReport.load_from_file(p)
            assert r is not None
            assert len(r.stages) == 2
            assert r.stages["fact"]["status"] == "passed"
        finally:
            p.unlink(missing_ok=True)
