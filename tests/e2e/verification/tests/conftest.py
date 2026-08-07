"""Shared fixtures for verification pipeline tests."""

from pathlib import Path
import sys
import tempfile
from typing import Generator

import pytest

_HERE = Path(__file__).resolve().parent  # verification/tests/
_VERIFICATION = _HERE.parent
import sys as _b_sys
from pathlib import Path as _b_Path
for _b_d in _b_Path(__file__).resolve().parents:
    if (_b_d / "_path.py").exists():
        if str(_b_d) not in _b_sys.path:
            _b_sys.path.insert(0, str(_b_d))
        break
from _path import foundation_root

_FOUNDATION_DLL = foundation_root()
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.orchestration.context import ChunkContext


@pytest.fixture
def chunk_ctx() -> ChunkContext:
    """Default ChunkContext for testing with temporary paths."""
    return ChunkContext(
        slug="test-chunk",
        assembly="Test.Assembly",
        chunk_dir=Path("/tmp/test-chunks/test-chunk"),
        foundation_dir=Path("/tmp/test-foundation"),
    )


@pytest.fixture
def temp_chunk_ctx() -> Generator[ChunkContext, None, None]:
    """ChunkContext backed by a real temp directory with chunk structure."""
    with tempfile.TemporaryDirectory() as td:
        base = Path(td)
        chunk_dir = base / "chunks" / "temp-chunk"
        chunk_dir.mkdir(parents=True)
        (chunk_dir / "managed").mkdir()
        (chunk_dir / "managed" / "subjects").mkdir()
        (chunk_dir / "native").mkdir()
        (chunk_dir / "results").mkdir()
        ctx = ChunkContext(
            slug="temp-chunk",
            assembly="Test.Assembly",
            chunk_dir=chunk_dir,
            foundation_dir=base,
        )
        yield ctx
