"""Side-effect-free loader for `config/pipeline-config.yaml`.

Lives in `_pipeline/` (rather than inline in `chunk_pipeline.py`) so it can be
unit-tested: importing `chunk_pipeline` triggers an SDK build and other
import-time work, which makes its internals impractical to test directly.

Kept dependency-free (no pyyaml) to match the original: the config uses only
simple `key: value` and nested mappings.
"""
from __future__ import annotations

import re
from pathlib import Path


def load_pipeline_config(path: Path) -> dict:
    """Parse the pipeline config into a plain dict.

    Recognised shape::

        pipeline:
          defaultMode: standard        # scalar -> config["defaultMode"]
          timeouts:                    # section
            codegen_aot: 600           #   -> config["timeouts"]["codegen_aot"]
          chunks:                      # section
            <slug>:                    #   -> config["chunks"][<slug>]
              assemblyDirs: "..."      #     -> {...}[assemblyDirs]

    Indentation is read from the RAW line.  The previous implementation did
    ``stripped = line.strip()`` and then tested
    ``not stripped.startswith(" ")`` to decide whether a key was top-level —
    a test that can never be true, because ``strip()`` has already removed the
    leading whitespace.  Every nested key was therefore treated as top-level,
    ``current_section`` was reset on each line, the ``chunks:`` block was never
    entered, and ``config["chunks"]`` came out empty.  The user-visible effect
    was that every per-chunk ``assemblyDirs`` was silently ignored: no
    ``--assembly-dir`` ever reached the codegen, so cross-assembly generic
    demands could never resolve their target assemblies.
    """
    config: dict = {}
    if not path.exists():
        return config

    text = path.read_text(encoding="utf-8")
    current_section: str | None = None
    current_subsection: str | None = None
    current_chunk: str | None = None
    timeouts: dict[str, int] = {}
    chunks: dict[str, dict] = {}

    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue

        # Indentation MUST come from the raw line, not from `stripped`.
        raw_indent = len(line) - len(line.lstrip())

        if raw_indent == 0:
            m = re.match(r"^([\w-]+):", stripped)
            if m:
                current_section = m.group(1)
                current_subsection = None
                current_chunk = None
            continue

        if current_section != "pipeline":
            continue

        if raw_indent == 2:
            m = re.match(r"^([\w-]+):\s*(.*)$", stripped)
            if m:
                key, val = m.group(1), m.group(2).strip()
                if val:
                    if val.isdigit():
                        config[key] = int(val)
                    elif val.lower() in ("true", "false"):
                        config[key] = val.lower() == "true"
                    else:
                        config[key] = val
                else:
                    # Section header (e.g. `timeouts:`, `chunks:`)
                    current_subsection = key
            continue

        if current_subsection == "timeouts":
            if raw_indent == 4:
                m = re.match(r"^([\w-]+):\s*(\d+)\s*$", stripped)
                if m:
                    timeouts[m.group(1)] = int(m.group(2))
            continue

        if current_subsection == "chunks":
            if raw_indent == 4:
                m = re.match(r"^([\w-]+):\s*$", stripped)
                if m:
                    current_chunk = m.group(1)
                    chunks.setdefault(current_chunk, {})
                continue
            if raw_indent == 6 and current_chunk is not None:
                m = re.match(r"^([\w-]+):\s*(.*)$", stripped)
                if m:
                    val = m.group(2).strip().strip('"').strip("'")
                    chunks[current_chunk][m.group(1)] = val
                continue

    if timeouts:
        config["timeouts"] = timeouts
    if chunks:
        config["chunks"] = chunks
    return config
