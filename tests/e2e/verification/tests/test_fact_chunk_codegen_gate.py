"""Regression tests for T4.1 kCodegenFailureCount gate.

Verifies that a chunk whose generated source carries a nonzero
kCodegenFailureCount is flagged at the fact stage, and that the
per-chunk tolerance config is respected.
"""
from __future__ import annotations

import json
import re

from stages.fact_chunk import _read_codegen_failure_count, _codegen_failure_tolerance

# ── Test helpers ──────────────────────────────────────────────────────


def _make_ctx(chunk_dir, extra_config: dict | None = None) -> object:
    """Minimal ChunkContext stand-in with a chunk_dir."""
    from pathlib import Path
    import tempfile
    from verification.orchestration.context import ChunkContext

    root = Path(chunk_dir)
    config = root / "chunk.json"
    if not config.exists() and extra_config is not None:
        config.write_text(json.dumps(extra_config), encoding="utf-8")
    elif config.exists() and extra_config is not None:
        existing = json.loads(config.read_text(encoding="utf-8"))
        existing.update(extra_config)
        config.write_text(json.dumps(existing), encoding="utf-8")

    ctx = ChunkContext.__new__(ChunkContext)
    ctx.chunk_dir = root
    return ctx


def _write_subject_file(chunk_dir: str, symbol_line: str | None) -> None:
    """Write a native-aot.generated.cpp into subjects/."""
    from pathlib import Path
    subjects_dir = Path(chunk_dir) / "native" / "subjects"
    subjects_dir.mkdir(parents=True, exist_ok=True)
    gen_cpp = subjects_dir / "native-aot.generated.cpp"
    lines = [
        "// auto-generated",
        'extern "C" void SomeMethod(void) noexcept { }',
    ]
    if symbol_line:
        lines.append(symbol_line)
    gen_cpp.write_text("\n".join(lines), encoding="utf-8")


# ── _read_codegen_failure_count ───────────────────────────────────────


def test_absent_file_returns_zero(tmp_path):
    """Missing subjects dir → 0 (not None, not crash)."""
    ctx = _make_ctx(str(tmp_path / "empty"))
    assert _read_codegen_failure_count(ctx) == 0


def test_empty_subjects_dir_returns_zero(tmp_path):
    """Present but empty subjects dir → 0."""
    (tmp_path / "native" / "subjects").mkdir(parents=True)
    ctx = _make_ctx(str(tmp_path))
    assert _read_codegen_failure_count(ctx) == 0


def test_no_symbol_returns_zero(tmp_path):
    """Generated file exists but has no kCodegenFailureCount → 0."""
    _write_subject_file(str(tmp_path), None)
    ctx = _make_ctx(str(tmp_path))
    assert _read_codegen_failure_count(ctx) == 0


def test_maximal_symbol_line_is_parsed(tmp_path):
    """The exact line format the generator emits is matched."""
    _write_subject_file(str(tmp_path),
        '// Codegen stub count — pipeline checks this\nextern "C" const int kCodegenFailureCount = 42;')
    ctx = _make_ctx(str(tmp_path))
    assert _read_codegen_failure_count(ctx) == 42


def test_zero_value_is_parsed_correctly(tmp_path):
    _write_subject_file(str(tmp_path),
        'extern "C" const int kCodegenFailureCount = 0;')
    ctx = _make_ctx(str(tmp_path))
    # The generator only emits the line when count > 0, but the parser
    # should still handle an explicit 0 if one appears.
    assert _read_codegen_failure_count(ctx) == 0


def test_multiple_subject_files_are_summed(tmp_path):
    """If the chunk has multiple generated files, sum all occurrences."""
    _write_subject_file(str(tmp_path),
        'extern "C" const int kCodegenFailureCount = 10;')
    # Add a second page file
    p2 = (tmp_path / "native" / "subjects" / "native-aot.generated.page2.cpp")
    p2.write_text(
        '// second page\nextern "C" const int kCodegenFailureCount = 5;',
        encoding="utf-8")
    ctx = _make_ctx(str(tmp_path))
    assert _read_codegen_failure_count(ctx) == 15


# ── _codegen_failure_tolerance ────────────────────────────────────────


def test_tolerance_defaults_to_zero(tmp_path):
    ctx = _make_ctx(str(tmp_path))
    assert _codegen_failure_tolerance(ctx) == 0


def test_tolerance_from_chunk_config(tmp_path):
    (tmp_path / "chunk.json").write_text(
        json.dumps({"codegenFailureTolerance": 5}), encoding="utf-8")
    ctx = _make_ctx(str(tmp_path))
    assert _codegen_failure_tolerance(ctx) == 5


def test_bool_tolerance_is_rejected(tmp_path):
    (tmp_path / "chunk.json").write_text(
        json.dumps({"codegenFailureTolerance": True}), encoding="utf-8")
    ctx = _make_ctx(str(tmp_path))
    assert _codegen_failure_tolerance(ctx) == 0


def test_negative_tolerance_is_rejected(tmp_path):
    (tmp_path / "chunk.json").write_text(
        json.dumps({"codegenFailureTolerance": -1}), encoding="utf-8")
    ctx = _make_ctx(str(tmp_path))
    assert _codegen_failure_tolerance(ctx) == 0