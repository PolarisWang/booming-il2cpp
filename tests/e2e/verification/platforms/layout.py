"""Build-tree layout — where a build system puts its outputs.

Pure path arithmetic.  No platform checks, no subprocess calls: a layout answers
"given a build directory and config, where is the object for X / the executable"
and nothing else.  That keeps it trivially unit-testable (see
``verification/tests/test_platform_layout.py``) and lets the hotupdate stage
stop hardcoding the Visual Studio generator's directory names.
"""

from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class BuildTreeLayout:
    """Where the build system places objects and the executable.

    A layout is deliberately dumb.  If a caller needs to *decide* something
    based on the build system, that decision belongs in the stage or in
    ``patch_link``, not here.
    """

    name: str

    def object_dir(self, build_dir: Path, config: str) -> Path:
        """Directory holding the objects for the ``chaos_entry`` target."""
        raise NotImplementedError

    def executable(self, build_dir: Path, config: str, stem: str) -> Path:
        """Path of the linked executable for target ``stem``."""
        raise NotImplementedError

    def object_glob(self, build_dir: Path, config: str, stem: str) -> str:
        """Glob matching the object(s) compiled from ``stem.cpp``.

        Returned as a string because callers hand it straight to ``glob.glob``.
        Callers must not assume the result is non-empty: a generator that
        compiles and links in one step (or a stale build tree) legitimately
        yields nothing, and the caller distinguishes that case.
        """
        raise NotImplementedError


class MsBuildLayout(BuildTreeLayout):
    """Visual Studio generator: nested ``<target>.dir/<Config>/`` folders.

    Layout::

        build/chaos_entry.dir/RelWithDebInfo/patch-host-arrays.cpp.obj
        build/RelWithDebInfo/chaos_entry.exe
    """

    def __init__(self) -> None:
        super().__init__(name="msbuild")

    def object_dir(self, build_dir: Path, config: str) -> Path:
        return build_dir / "chaos_entry.dir" / config

    def executable(self, build_dir: Path, config: str, stem: str) -> Path:
        return build_dir / config / f"{stem}.exe"

    def object_glob(self, build_dir: Path, config: str, stem: str) -> str:
        # MSVC names objects "<source>.obj", so the stem is a prefix.
        return str(self.object_dir(build_dir, config) / f"{stem}*")


class NinjaLayout(BuildTreeLayout):
    """Ninja / Unix Makefiles: flat ``CMakeFiles/<target>.dir/`` and no suffix.

    Layout::

        build/CMakeFiles/chaos_entry.dir/patch-host-arrays.cpp.o
        build/chaos_entry

    Ninja and Makefiles share this shape, which is why they share a layout —
    the difference between them is *how the build is driven*, which this class
    deliberately does not model.
    """

    def __init__(self) -> None:
        super().__init__(name="ninja")

    def object_dir(self, build_dir: Path, config: str) -> Path:
        # Single-config generators ignore `config`; it is accepted anyway so the
        # call sites do not have to branch on layout kind.
        return build_dir / "CMakeFiles" / "chaos_entry.dir"

    def executable(self, build_dir: Path, config: str, stem: str) -> Path:
        return build_dir / stem

    def object_glob(self, build_dir: Path, config: str, stem: str) -> str:
        return str(self.object_dir(build_dir, config) / f"{stem}*")


def layout_for_platform(platform: str | None = None) -> BuildTreeLayout:
    """Pick a layout for ``platform`` (defaults to the running platform).

    Windows gets the Visual Studio generator layout because the nightly drives
    MSBuild there; everything else uses the Ninja/Make layout the Linux agent
    and the cross-compilation presets produce.
    """
    plat = sys.platform if platform is None else platform
    return MsBuildLayout() if plat.startswith("win") else NinjaLayout()
