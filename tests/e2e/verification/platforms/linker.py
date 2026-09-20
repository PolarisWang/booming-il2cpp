"""Linker capabilities — the only thing hotupdate needs to know about a toolchain.

hotupdate's distinct-linking trick relies on the linker tolerating two object
files that define the same symbol, and keeping the definition from the object it
saw **first**.  Both halves matter:

  * tolerating duplicates        — otherwise the link is a hard error
  * keeping the FIRST definition — otherwise source order cannot decide the
                                   winner and the trick has no lever

Both MSVC and GNU ld satisfy this; the flag names differ and are the only
platform-specific part.
"""

from __future__ import annotations

import os
import shutil
import sys
from dataclasses import dataclass

# MSVC: /FORCE:MULTIPLE reports LNK4006 for the duplicates and keeps the first.
_MSVC_DUPLICATE_FLAG = "/FORCE:MULTIPLE"

# GNU ld / lld: --allow-multiple-definition.  Measured behaviour is identical to
# MSVC's — the first definition seen on the command line wins.  Verified by
# linking two objects defining the same symbol in both orders (see the unit
# tests for the invariant, and the wiki note for the measurement).
_GNU_DUPLICATE_FLAG = "-Wl,--allow-multiple-definition"


@dataclass(frozen=True)
class LinkerProfile:
    """What the active linker can do.

    ``duplicate_symbol_flag is None`` means this linker cannot express
    "tolerate the duplicate and keep the first".  Callers must then fall back to
    a strategy that avoids duplicate symbols entirely (see ``patch_link``);
    treating a missing flag as an error keeps a silent non-patch build from
    being reported as a pass.
    """

    name: str
    duplicate_symbol_flag: str | None

    @property
    def supports_distinct_linking(self) -> bool:
        return self.duplicate_symbol_flag is not None


def _msvc_is_active() -> bool:
    """Whether a real MSVC toolchain is driving the build.

    ``sys.platform == 'win32'`` alone is not enough: clang-cl and MinGW set it
    too and do not accept ``/FORCE:MULTIPLE``.  Require cl.exe/link.exe on PATH
    or the environment markers a VS-imported shell sets.
    """
    if not sys.platform.startswith("win"):
        return False
    if shutil.which("cl.exe") or shutil.which("link.exe"):
        return True
    return any(
        os.environ.get(v)
        for v in ("VCToolsInstallDir", "VCINSTALLDIR", "VSINSTALLDIR", "VisualStudioVersion")
    )


def linker_for_platform(platform: str | None = None) -> LinkerProfile:
    """Identify the active linker.

    Returning a profile with ``duplicate_symbol_flag=None`` for Windows without
    MSVC is intentional: clang-cl/MinGW cannot do distinct linking, and the
    caller must notice rather than proceed and produce a non-patch build.
    """
    plat = sys.platform if platform is None else platform

    if plat.startswith("win"):
        if _msvc_is_active():
            return LinkerProfile("msvc", _MSVC_DUPLICATE_FLAG)
        return LinkerProfile("windows-non-msvc", None)

    if plat.startswith(("linux", "freebsd", "openbsd", "netbsd")):
        return LinkerProfile("gnu-ld", _GNU_DUPLICATE_FLAG)

    if plat == "darwin":
        # Apple's ld has no equivalent of --allow-multiple-definition; a
        # duplicate strong symbol is an error there.
        return LinkerProfile("apple-ld", None)

    return LinkerProfile(f"unknown:{plat}", None)
