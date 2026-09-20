"""Unit tests for the patch-link strategies and the runtime activation check.

The runtime check replaced a post-hoc parse of the MSBuild build log.  The tests
below pin the properties that made the replacement safe:

  * it must fail (not pass) when the patch did not take effect,
  * it must tolerate engine log noise / missing fields without crashing,
  * it must not require the build system's log format at all.

Note it inspects the stage's *already-parsed* --hotupdate payload rather than
running the binary itself — a bare `--hotupdate` has no patch data to apply, so
re-running it would report zero patches on a healthy build.
"""

from __future__ import annotations

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


class TestVerifyPatchActive:
    def test_passes_when_patches_applied(self):
        ok, detail = verify_patch_active(
            {"baselineFact": [], "patchedFact": [{"si": 0}, {"si": 1}]},
            expected_patch_count=2,
        )
        assert ok, detail
        assert "2" in detail

    def test_fails_when_zero_patches_applied(self):
        """The sentinel winning the link must be a failure, not a pass.

        This is the case the whole distinct-linking machinery exists to prevent:
        the binary runs, prints a well-formed payload, and simply did nothing.
        """
        ok, detail = verify_patch_active({"baselineFact": [], "patchedFact": []})
        assert not ok
        assert "0 patches" in detail

    def test_fails_when_patched_fact_absent(self):
        """A payload without patchedFact means the run never reached the patch phase."""
        ok, detail = verify_patch_active({"baselineFact": [{"si": 0}]})
        assert not ok
        assert "patchedFact" in detail

    def test_fails_on_empty_payload(self):
        ok, detail = verify_patch_active({})
        assert not ok

    def test_flags_partial_patch_data(self):
        ok, detail = verify_patch_active(
            {"patchedFact": [{"si": 0}]}, expected_patch_count=5
        )
        assert not ok
        assert "fewer" in detail

    def test_accepts_any_nonzero_when_no_expectation(self):
        ok, _ = verify_patch_active({"patchedFact": [{"si": 0}]})
        assert ok

    def test_does_not_depend_on_build_log_format(self):
        """Regression guard for the reason this check exists at all.

        The old implementation inferred compile order from MSBuild's log text.
        This check consumes a parsed dict — no log input anywhere — which is what
        makes it portable to Ninja/Make and to future platforms.
        """
        ok, _ = verify_patch_active({"patchedFact": [{"si": 0}]})
        assert ok
