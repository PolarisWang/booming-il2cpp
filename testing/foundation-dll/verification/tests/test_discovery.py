"""Tests for verification.orchestration.discovery — chunk slug discovery."""

from pathlib import Path
import json
import sys
import tempfile

_HERE = Path(__file__).resolve().parent
_VERIFICATION = _HERE.parent
_FOUNDATION_DLL = _VERIFICATION.parent
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.orchestration.discovery import discover_chunks


class TestDiscoverChunks:
    """Chunk discovery is the entry point for pipeline orchestration."""

    def test_discover_from_valid_partition(self):
        with tempfile.TemporaryDirectory() as td:
            dll_dir = Path(td) / "_dll"
            dll_dir.mkdir()
            partition = {
                "chunks": [
                    {"slug": "system", "namespaces": ["System"]},
                    {"slug": "collections", "namespaces": ["System.Collections"]},
                    {"slug": "numerics", "namespaces": ["System.Numerics"]},
                ]
            }
            (dll_dir / "namespace-partition.json").write_text(
                json.dumps(partition), encoding="utf-8"
            )
            slugs = discover_chunks(
                assembly="Test",
                foundation_dir=Path(td),
            )
            assert slugs == ["system", "collections", "numerics"]  # order matches partition JSON

    def test_discover_no_partition_file(self):
        with tempfile.TemporaryDirectory() as td:
            slugs = discover_chunks(
                assembly="Test",
                foundation_dir=Path(td),
            )
            assert slugs == []

    def test_discover_bad_json(self):
        with tempfile.TemporaryDirectory() as td:
            dll_dir = Path(td) / "_dll"
            dll_dir.mkdir()
            (dll_dir / "namespace-partition.json").write_text(
                "{bad json", encoding="utf-8"
            )
            slugs = discover_chunks(
                assembly="Test",
                foundation_dir=Path(td),
            )
            assert slugs == []

    def test_discover_empty_chunks(self):
        with tempfile.TemporaryDirectory() as td:
            dll_dir = Path(td) / "_dll"
            dll_dir.mkdir()
            (dll_dir / "namespace-partition.json").write_text(
                json.dumps({"chunks": []}), encoding="utf-8"
            )
            slugs = discover_chunks(
                assembly="Test",
                foundation_dir=Path(td),
            )
            assert slugs == []
