"""Tests for nightly worklist discovery.

The bug these cover: `namespace-partition.json` is regenerated from the DLL on
every run and lists every namespace the DLL *has*, while the chunk source under
`chunks/<slug>/managed/` is a committed translation-tree snapshot that lags
behind.  A partition entry with no matching source directory is unrunnable —
ATG is handed a namespace filter but no input, emits 0 subjects, no entry.exe is
built, and the chunk fails as an opaque `unknown`.  Build #311 counted 5 such
guaranteed failures out of 19, including all three System.Xml.ReaderWriter
chunks.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

_HERE = Path(__file__).resolve()
_VERIFY = _HERE.parents[1]          # .../verification/
for _p in (_VERIFY, _VERIFY.parent):
    if str(_p) not in sys.path:
        sys.path.insert(0, str(_p))

from verification.nightly.worklist import discover_worklist  # noqa: E402


class _Config:
    """Minimal stand-in for NightlyConfig (only these two fields are read)."""

    def __init__(self, foundation_dir: Path, assembly_filter=None):
        self.foundation_dir = str(foundation_dir)
        self.assembly_filter = assembly_filter or []


def _make_tree(root: Path, assembly: str, chunks: list[tuple[str, bool]]) -> Path:
    """Build a fake foundation tree.

    chunks: list of (slug, has_source) — has_source controls whether
    chunks/<slug>/managed/ exists.
    """
    entry = root / assembly
    (entry / "_dll").mkdir(parents=True, exist_ok=True)
    (entry / "_dll" / "namespace-partition.json").write_text(
        json.dumps({"chunks": [{"slug": s} for s, _ in chunks]}),
        encoding="utf-8",
    )
    for slug, has_source in chunks:
        if has_source:
            (entry / "chunks" / slug / "managed").mkdir(parents=True, exist_ok=True)
    return root


def test_schedules_only_chunks_with_source(tmp_path):
    """A declared chunk with no chunks/<slug>/managed/ is not scheduled."""
    _make_tree(tmp_path, "System.Fake", [("real", True), ("ghost", False)])

    items = discover_worklist(_Config(tmp_path))

    assert [i.slug for i in items] == ["real"]


def test_skips_reported_not_silent(tmp_path, capsys):
    """The skip is announced, so a vanished chunk is visible in the log."""
    _make_tree(tmp_path, "System.Fake", [("ghost", False)])

    discover_worklist(_Config(tmp_path))

    out = capsys.readouterr().out
    assert "System.Fake__ghost" in out
    assert "skipped 1" in out


def test_real_chunks_unaffected(tmp_path):
    """Chunks that do have source are scheduled exactly as before."""
    _make_tree(tmp_path, "System.Fake", [("a", True), ("b", True)])

    items = discover_worklist(_Config(tmp_path))

    assert [i.slug for i in items] == ["a", "b"]


def test_assembly_filter_still_applies(tmp_path):
    """Filtering by assembly is unaffected by the source check."""
    _make_tree(tmp_path, "System.One", [("a", True)])
    _make_tree(tmp_path, "System.Two", [("b", True)])

    items = discover_worklist(_Config(tmp_path, assembly_filter=["System.Two"]))

    assert [i.key for i in items] == ["System.Two__b"]


def test_missing_partition_is_skipped(tmp_path):
    """An assembly with no partition manifest contributes nothing (unchanged)."""
    (tmp_path / "System.Bare").mkdir()

    assert discover_worklist(_Config(tmp_path)) == []
