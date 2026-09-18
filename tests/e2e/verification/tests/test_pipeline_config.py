"""Regression tests for the pipeline-config parser.

The parser used to decide "is this key top-level?" with
``not stripped.startswith(" ")`` — evaluated on the STRIPPED line, so the test
could never be true.  Every nested key was treated as top-level,
``current_section`` reset on each line, the ``chunks:`` block was never
entered, and ``config["chunks"]`` came out empty.

The user-visible consequence was silent and repo-wide: ``ctx.assembly_dirs``
was always empty, so no ``--assembly-dir`` ever reached the codegen, so
cross-assembly generic instantiation demands could never resolve their target
assemblies.  Nothing failed loudly — the config was simply ignored.

These tests pin the parse of the real shipped config as well as the nested
shapes.
"""
from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent  # verification/tests/
for _d in _HERE.parents:
    if (_d / "_path.py").exists():
        if str(_d) not in sys.path:
            sys.path.insert(0, str(_d))
        break
from _path import foundation_root  # noqa: E402

_FOUNDATION_DLL = foundation_root()
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from _pipeline.pipeline_config import load_pipeline_config  # noqa: E402


def _write(tmp_path: Path, body: str) -> Path:
    p = tmp_path / "pipeline-config.yaml"
    # CRLF on purpose: the shipped config uses CRLF, and the parser must not
    # care.
    p.write_bytes(body.replace("\n", "\r\n").encode("utf-8"))
    return p


def test_scalar_keys_are_read(tmp_path):
    cfg = load_pipeline_config(_write(tmp_path, """\
pipeline:
  defaultMode: standard
"""))
    assert cfg["defaultMode"] == "standard"


def test_timeouts_section_is_read(tmp_path):
    cfg = load_pipeline_config(_write(tmp_path, """\
pipeline:
  timeouts:
    codegen_aot: 600
    fact_aot: 700
"""))
    assert cfg["timeouts"] == {"codegen_aot": 600, "fact_aot": 700}


def test_chunks_section_is_read(tmp_path):
    """The core regression: nested `chunks:` must survive parsing."""
    cfg = load_pipeline_config(_write(tmp_path, """\
pipeline:
  chunks:
    numerics:
      assemblyDirs: ""
    threading-tasks:
      assemblyDirs: "C:/runtime/10.0.6"
"""))
    assert cfg["chunks"]["threading-tasks"]["assemblyDirs"] == "C:/runtime/10.0.6"
    assert cfg["chunks"]["numerics"]["assemblyDirs"] == ""


def test_sections_do_not_bleed_into_each_other(tmp_path):
    # A scalar after a section header must not be swallowed by that section,
    # and a later section must not inherit the earlier one's entries.
    cfg = load_pipeline_config(_write(tmp_path, """\
pipeline:
  defaultMode: standard
  timeouts:
    codegen_aot: 600
  chunks:
    alpha:
      assemblyDirs: "a"
"""))
    assert cfg["defaultMode"] == "standard"
    assert cfg["timeouts"] == {"codegen_aot": 600}
    assert list(cfg["chunks"]) == ["alpha"]


def test_comments_and_blank_lines_ignored(tmp_path):
    cfg = load_pipeline_config(_write(tmp_path, """\
# leading comment
pipeline:

  # nested comment
  defaultMode: standard
"""))
    assert cfg["defaultMode"] == "standard"


def test_quoted_values_are_unquoted(tmp_path):
    cfg = load_pipeline_config(_write(tmp_path, """\
pipeline:
  chunks:
    beta:
      assemblyDirs: 'crypto-refs'
"""))
    assert cfg["chunks"]["beta"]["assemblyDirs"] == "crypto-refs"


def test_missing_file_returns_empty(tmp_path):
    assert load_pipeline_config(tmp_path / "nope.yaml") == {}


def test_shipped_config_parses_chunks():
    """The real config must expose per-chunk assemblyDirs.

    This is the assertion that would have caught the original bug: the shipped
    file declares six chunks with assemblyDirs, and every one of them used to be
    dropped on the floor.

    Locating the file: `foundation_root()` honours CHAOS_FOUNDATION_DLL and else
    walks up for the `config/pipeline-config.yaml` marker, so it resolves to the
    authoritative tree.  When the marker cannot be found it falls back to
    parent-of-package, which after the L6/L7 relocate can be the zombie
    `tests/e2e` tree that has no config.  Search the candidates and skip if the
    config genuinely is not present, rather than asserting on a mis-resolved
    path.
    """
    candidates = [
        _FOUNDATION_DLL / "config" / "pipeline-config.yaml",
        # foundation_root() falls back to parent-of-package when the marker walk
        # fails, which after the L6/L7 relocate can land on the zombie tests/e2e
        # tree.  The authoritative tree is a sibling of tests/.
        _HERE.parents[3] / "testing" / "foundation-dll" / "config" / "pipeline-config.yaml",
    ]
    shipped = next((c for c in candidates if c.exists()), None)
    if shipped is None:
        import pytest
        pytest.skip("pipeline-config.yaml not found from the resolved roots")

    cfg = load_pipeline_config(shipped)

    assert "chunks" in cfg, f"shipped config ({shipped}) must parse its `chunks:` block"
    chunks = cfg["chunks"]
    assert "numerics" in chunks
    # A chunk that genuinely declares a non-empty assemblyDirs.
    assert chunks.get("global-ns", {}).get("assemblyDirs"), (
        "global-ns declares assemblyDirs in the shipped config; an empty "
        "value here means the nested parse regressed")
    assert cfg.get("timeouts", {}).get("codegen_aot") == 600
