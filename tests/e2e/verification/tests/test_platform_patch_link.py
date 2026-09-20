"""Unit tests for the patch-link strategies and the runtime activation check.

The runtime check replaced a post-hoc parse of the MSBuild build log.  The tests
below pin the properties that made the replacement safe:

  * it must fail (not pass) when the patch did not take effect,
  * it must not be fooled by engine log lines printed before the JSON payload,
  * it must not require the build system's log format at all.
"""

from __future__ import annotations

import json
import stat
import sys
from pathlib import Path

_HERE = Path(__file__).resolve()
_VERIFY = _HERE.parents[1]
for _p in (_VERIFY, _VERIFY.parent):
    if str(_p) not in sys.path:
        sys.path.insert(0, str(_p))

from verification.platforms.patch_link import (  # noqa: E402
    verify_patch_active,
)


def _fake_entry(tmp_path: Path, stdout: str, exit_code: int = 0) -> Path:
    """Create an executable that prints `stdout` (mimics entry.exe --hotupdate).

    Written as a real script rather than a mock so the check exercises the actual
    subprocess + parse path, including the "log lines precede JSON" case.
    """
    exe = tmp_path / "entry.exe"
    payload = json.dumps(stdout)
    exe.write_text(f"#!/bin/sh\nprintf '%s' {payload}\nexit {exit_code}\n")
    exe.chmod(exe.stat().st_mode | stat.S_IEXEC | stat.S_IXGRP | stat.S_IXOTH)
    return exe


class TestVerifyPatchActive:
    def test_passes_when_patches_applied(self, tmp_path: Path):
        exe = _fake_entry(
            tmp_path,
            '{"baselineFact":[],"patchedFact":[{"si":0},{"si":1}]}',
        )
        ok, detail = verify_patch_active(exe, expected_patch_count=2)
        assert ok, detail
        assert "2" in detail

    def test_fails_when_zero_patches_applied(self, tmp_path: Path):
        """The sentinel winning the link must be a failure, not a pass.

        This is the case the whole distinct-linking machinery exists to prevent:
        the binary runs, prints a well-formed payload, and simply did nothing.
        """
        exe = _fake_entry(tmp_path, '{"baselineFact":[],"patchedFact":[]}')
        ok, detail = verify_patch_active(exe, expected_patch_count=2)
        assert not ok
        assert "0 patches" in detail

    def test_tolerates_engine_log_lines_before_json(self, tmp_path: Path):
        """Engine warnings go to stdout ahead of the payload; do not assume position 0."""
        exe = _fake_entry(
            tmp_path,
            '[WARN][ABI] module validation failed\n'
            '{"baselineFact":[],"patchedFact":[{"si":0}]}',
        )
        ok, detail = verify_patch_active(exe, expected_patch_count=1)
        assert ok, detail

    def test_fails_on_unparsable_payload(self, tmp_path: Path):
        exe = _fake_entry(tmp_path, "not json at all")
        ok, detail = verify_patch_active(exe, expected_patch_count=1)
        assert not ok
        assert "JSON" in detail or "json" in detail

    def test_fails_when_binary_missing(self, tmp_path: Path):
        ok, detail = verify_patch_active(tmp_path / "nope.exe", expected_patch_count=1)
        assert not ok
        assert "missing" in detail

    def test_does_not_depend_on_build_log_format(self, tmp_path: Path):
        """Regression guard for the reason this check exists at all.

        The old implementation inferred compile order from MSBuild's log text.
        This check must work with no log input whatsoever — that is what makes
        it portable to Ninja/Make and to future platforms.
        """
        exe = _fake_entry(tmp_path, '{"patchedFact":[{"si":0}]}')
        ok, _ = verify_patch_active(exe, expected_patch_count=1)
        assert ok
