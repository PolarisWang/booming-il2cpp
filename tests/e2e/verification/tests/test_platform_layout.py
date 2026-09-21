"""Unit tests for verification/platform/ — build-tree layout.

These are pure path-arithmetic tests: no cmake, no compiler, no subprocess.  A
layout that needs a real build to be testable is a layout that is doing too
much.
"""

from __future__ import annotations

import sys
from pathlib import Path

_HERE = Path(__file__).resolve()
_VERIFY = _HERE.parents[1]
for _p in (_VERIFY, _VERIFY.parent):
    if str(_p) not in sys.path:
        sys.path.insert(0, str(_p))

from verification.platforms.layout import (  # noqa: E402
    MsBuildLayout,
    NinjaLayout,
    layout_for_platform,
)


class TestMsBuildLayout:
    def test_object_dir_matches_visual_studio_generator(self):
        assert MsBuildLayout().object_dir(Path("/b"), "RelWithDebInfo") == Path(
            "/b/chaos_entry.dir/RelWithDebInfo"
        )

    def test_executable_is_in_config_dir_with_exe_suffix(self):
        assert MsBuildLayout().executable(Path("/b"), "RelWithDebInfo", "chaos_entry") == Path(
            "/b/RelWithDebInfo/chaos_entry.exe"
        )

    def test_object_glob_is_a_prefix_match(self):
        # MSVC names objects "<source>.obj", so matching on the stem alone would
        # miss them; the glob must be a prefix.
        g = MsBuildLayout().object_glob(Path("/b"), "RelWithDebInfo", "patch-host-arra")
        assert g == "/b/chaos_entry.dir/RelWithDebInfo/patch-host-arra*"


class TestNinjaLayout:
    def test_object_dir_is_flat_cmakefiles(self):
        assert NinjaLayout().object_dir(Path("/b"), "RelWithDebInfo") == Path(
            "/b/CMakeFiles/chaos_entry.dir"
        )

    def test_executable_has_no_suffix(self):
        assert NinjaLayout().executable(Path("/b"), "RelWithDebInfo", "chaos_entry") == Path(
            "/b/chaos_entry"
        )

    def test_config_is_accepted_but_ignored(self):
        """Single-config generators ignore it; call sites must not have to branch."""
        layout = NinjaLayout()
        assert layout.object_dir(Path("/b"), "Debug") == layout.object_dir(Path("/b"), "Release")

    def test_object_glob_uses_cpp_o_naming(self):
        g = NinjaLayout().object_glob(Path("/b"), "RelWithDebInfo", "patch-host-arra")
        assert g == "/b/CMakeFiles/chaos_entry.dir/patch-host-arra*"


class TestLayoutSelection:
    def test_windows_selects_msbuild(self):
        assert isinstance(layout_for_platform("win32"), MsBuildLayout)

    def test_linux_selects_ninja(self):
        assert isinstance(layout_for_platform("linux"), NinjaLayout)

    def test_darwin_selects_ninja(self):
        assert isinstance(layout_for_platform("darwin"), NinjaLayout)

    def test_defaults_to_running_platform(self):
        expected = MsBuildLayout if sys.platform.startswith("win") else NinjaLayout
        assert isinstance(layout_for_platform(), expected)
